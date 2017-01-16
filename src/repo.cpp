#include "repo.h"
#include "reggata_exceptions.h"
#include "db_key.h"
#include "status_code.h"
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <iostream>

Repo::Repo(const boost::filesystem::path& rootPath, const boost::filesystem::path& dbPath, bool initIfNotExists)
: _rootPath(rootPath), _dbPath(dbPath), _db(new Database(dbPath, initIfNotExists)),
_thread(&Repo::run, this) {
    if (!rootPath.is_absolute()) {
        throw ReggataException(boost::format("rootPath=%1% is not absolute") % rootPath);
    }
    if (!dbPath.is_absolute()) {
        throw ReggataException(boost::format("dbPath=%1% is not absolute") % dbPath);
    }
    if (initIfNotExists && !boost::filesystem::exists(rootPath)) {
        boost::filesystem::create_directories(rootPath);
    }

    for (auto&& entry : boost::filesystem::recursive_directory_iterator(_rootPath)) {

        auto p = std::mismatch(_dbPath.begin(), _dbPath.end(), entry.path().begin());
        if (p.first == _dbPath.end()) {
            std::cout << "SKIPPING " << entry.path() << std::endl;
            continue;
        } else {
            std::cout << "CREATE WATCHER " << entry.path() << std::endl;
        }
        if (entry.status().type() != boost::filesystem::file_type::directory_file) {
            continue;
        }
        createDirWatcherIfNeeded(entry.path());
    }
    createDirWatcherIfNeeded(rootPath);
}

void Repo::stop() {
    BOOST_LOG_TRIVIAL(info) << "Stopping Repo " << _rootPath;
    _stopCalled = true;
    _thread.interrupt();
    _thread.join();
    BOOST_LOG_TRIVIAL(info) << "Repo stopped " << _rootPath;
}

void Repo::run() {
    BOOST_LOG_TRIVIAL(info) << "Repo started " << _rootPath;
    while (!_stopCalled && !_thread.interruption_requested()) {
        try {
            auto cmd = _queue.dequeue();
            json::json result;
            try {
                result = cmd->execute();
            } catch (const StatusCodeException& ex) {
                result = {
                    {"code", ex.statusCode()},
                    {"msg", ex.what()}
                };
            } catch (const std::exception& ex) {
                result = {
                    {"code", StatusCode::SERVER_ERROR},
                    {"msg", ex.what()}
                };
            }
            cmd->sendResult(result);
        } catch (const boost::thread_interrupted& ex) {
            break;
        } catch (const std::exception& ex) {
            BOOST_LOG_TRIVIAL(error) << "std::exception " << ex.what();
        } catch (...) {
            BOOST_LOG_TRIVIAL(error) << "Unexpected exception in Repo::run";
        }
    }
    BOOST_LOG_TRIVIAL(info) << "Repo::run exited " << _rootPath;
}

boost::filesystem::path Repo::rootPath() const {
    return _rootPath;
}

void Repo::enqueueCmd(std::unique_ptr<CmdRepo> cmd) {
    _queue.enqueue(std::move(cmd));
}

void Repo::addTags(const boost::filesystem::path& fileAbs, const std::vector<std::string>& tags) {
    if (!boost::filesystem::exists(fileAbs)) {
        throw StatusCodeException(StatusCode::CLIENT_ERROR,
                (boost::format("Could not add tags, reason: file %1% does not exists")
                % fileAbs).str());
    }
    auto fileRel = makeRelativePath(fileAbs);
    auto fileId = getOrCreateFileId(fileRel);
    for (auto tag : tags) {
        addTag(fileId, tag);
    }
}

void Repo::removeTags(const boost::filesystem::path& fileAbs, const std::vector<std::string>& tags) {
    if (!boost::filesystem::exists(fileAbs)) {
        throw StatusCodeException(StatusCode::CLIENT_ERROR,
                (boost::format("Could not remove tags, reason: file %1% does not exists")
                % fileAbs).str());
    }
    auto fileRel = makeRelativePath(fileAbs);
    auto fileId = getOrCreateFileId(fileRel);
    for (auto tag : tags) {
        removeTag(fileId, tag);
    }
}

boost::filesystem::path Repo::makeRelativePath(const boost::filesystem::path& abs) const {
    boost::system::error_code ec;
    auto rel = boost::filesystem::relative(abs, _rootPath, ec);
    if (ec.value() != boost::system::errc::success) {
        throw ReggataException(ec.message());
    }
    return rel;
}

std::string Repo::getOrCreateFileId(const boost::filesystem::path& fileRel) {
    std::string fileId;
    auto found = getFileId(fileRel, &fileId);
    if (!found) {
        fileId = createFileId(fileRel);
    }
    return fileId;
}

bool Repo::getFileId(const boost::filesystem::path& fileRel, std::string* fileId) const {
    // find file_id - get value of record (file_path, /a/c, 2)
    auto db = _db->getDB();
    auto cfh = _db->getColumnFamilyHandle(Database::CF_FILE_PATH);
    auto s = db->Get(rocksdb::ReadOptions(), cfh, fileRel.string(), fileId);
    if (!s.ok() && !s.IsNotFound()) {
        throw StatusCodeException(StatusCode::SERVER_ERROR,
                (boost::format("Failed to get file_id of %1%, reason %2%")
                % fileRel.string() % s.ToString()).str());
    }
    return !s.IsNotFound();
}

std::string Repo::createFileId(const boost::filesystem::path& fileRel) {
    // Algorithm:
    // 1) generate next file_id
    // 2) Put three values: (file_path, /a/c, 2), (file, 2:path, /a/c), (file, 2:size, 234234)
    std::string fileId;
    auto db = _db->getDB();
    auto cfhFile = _db->getColumnFamilyHandle(Database::CF_FILE);
    bool needToRegenerateId = false;
    do {
        boost::uuids::uuid u = _uuidGenerator();
        fileId = boost::lexical_cast<std::string>(u);
        std::string filePath;
        auto s = db->Get(rocksdb::ReadOptions(), cfhFile, DBKey::join(fileId, "path"), &filePath);
        if (!s.ok() && !s.IsNotFound()) {
            throw StatusCodeException(StatusCode::SERVER_ERROR,
                    (boost::format("Failed to create file id for %1%, reason ")
                    % fileRel.string() % s.ToString()).str());
        }
        needToRegenerateId = s.ok();
    } while (needToRegenerateId);

    auto cfhFilePath = _db->getColumnFamilyHandle(Database::CF_FILE_PATH);
    rocksdb::WriteBatch wb;
    wb.Put(cfhFilePath, fileRel.string(), fileId);
    wb.Put(cfhFile, DBKey::join(fileId, "path"), fileRel.string());
    auto fileSizeStr = std::to_string(boost::filesystem::file_size(_rootPath / fileRel));
    wb.Put(cfhFile, DBKey::join(fileId, "size"), fileSizeStr);
    rocksdb::WriteOptions wo;
    wo.sync = true;
    auto st = db->Write(wo, &wb);
    if (!st.ok()) {
        throw StatusCodeException(StatusCode::SERVER_ERROR, (boost::format(
                "Failed to create new file entity for %1%, reason: %2%")
                % fileRel % st.ToString()).str());
    }
    return fileId;
}

void Repo::addTag(const std::string& fileId, const std::string& tag) {
    // Put two records (file_tag, 1:Tag1, "") and (tag_file, Tag1:1, "") for a tag 'Tag1'
    auto cfhFileTag = _db->getColumnFamilyHandle(Database::CF_FILE_TAG);
    auto cfhTagFile = _db->getColumnFamilyHandle(Database::CF_TAG_FILE);
    auto db = _db->getDB();
    rocksdb::WriteBatch wb;
    wb.Put(cfhFileTag, DBKey::join(fileId, tag), "");
    wb.Put(cfhTagFile, DBKey::join(tag, fileId), "");
    rocksdb::WriteOptions wo;
    wo.sync = true;
    auto st = db->Write(wo, &wb);
    if (!st.ok()) {
        throw StatusCodeException(StatusCode::SERVER_ERROR, (boost::format(
                "Failed to create new tag %1% of file %2%, reason: %3%")
                % tag % fileId % st.ToString()).str());
    }
}

void Repo::removeTag(const std::string& fileId, const std::string& tag) {
    // Delete two records (file_tag, 1:Tag1, "") and (tag_file, Tag1:1, "") for a tag 'Tag1'
    auto cfhFileTag = _db->getColumnFamilyHandle(Database::CF_FILE_TAG);
    auto cfhTagFile = _db->getColumnFamilyHandle(Database::CF_TAG_FILE);
    auto db = _db->getDB();
    rocksdb::WriteBatch wb;
    wb.Delete(cfhFileTag, DBKey::join(fileId, tag));
    wb.Delete(cfhTagFile, DBKey::join(tag, fileId));
    rocksdb::WriteOptions wo;
    wo.sync = true;
    auto st = db->Write(wo, &wb);
    if (!st.ok()) {
        throw StatusCodeException(StatusCode::SERVER_ERROR, (boost::format(
                "Failed to remove tag %1% of file %2%, reason: %3%")
                % tag % fileId % st.ToString()).str());
    }
}

FileInfo Repo::getFileInfo(const boost::filesystem::path& fileAbs) const {
    auto fileRel = makeRelativePath(fileAbs);
    std::string fileId;
    auto found = getFileId(fileRel, &fileId);
    if (!found) {
        FileInfo res;
        res._path = fileRel.string();
        res._size = boost::filesystem::file_size(fileAbs); // TODO: size is stored in DB. What should we do in case sizes are different?..
        return res;
    }
    auto res = getFileInfoById(fileId);
    return res;
}

FileInfo Repo::getFileInfoById(const std::string& fileId) const {
    FileInfo res;
    rocksdb::ReadOptions ro;
    ro.prefix_same_as_start = true;
    auto db = _db->getDB();
    auto cfh = _db->getColumnFamilyHandle(Database::CF_FILE_TAG);
    std::unique_ptr<rocksdb::Iterator> j(db->NewIterator(ro, cfh));
    for (j->Seek(fileId); j->Valid(); j->Next()) {
        auto key = j->key().ToString();
        auto p = DBKey::split(key);
        res._tags.push_back(p.second);
    }

    auto cfhFile = _db->getColumnFamilyHandle(Database::CF_FILE);

    std::string sizeStr;
    db->Get(ro, cfhFile, DBKey::join(fileId, "size"), &sizeStr);
    res._size = std::stoul(sizeStr);

    db->Get(ro, cfhFile, DBKey::join(fileId, "path"), &res._path);

    return res;
}

std::vector<FileInfo> Repo::getFileInfos(const std::unordered_set<std::string>& fileIds) const {
    std::vector<FileInfo> res;
    for (auto fileId : fileIds) {
        auto finfo = getFileInfoById(fileId);
        res.push_back(finfo);
    }
    return res;
}

std::unordered_set<std::string> Repo::findFileIds(const std::string& tag,
        const boost::filesystem::path& dirRel) const {
    std::unordered_set<std::string> result;
    rocksdb::ReadOptions ro;
    ro.prefix_same_as_start = true;
    auto db = _db->getDB();
    auto cfhTagFile = _db->getColumnFamilyHandle(Database::CF_TAG_FILE);
    auto cfhFile = _db->getColumnFamilyHandle(Database::CF_FILE);
    std::unique_ptr<rocksdb::Iterator> j(db->NewIterator(ro, cfhTagFile));
    for (j->Seek(tag); j->Valid(); j->Next()) {
        auto key = j->key().ToString();
        auto p = DBKey::split(key);

        std::string path;
        db->Get(ro, cfhFile, DBKey::join(p.second, "path"), &path);
        if (!Repo::isPrefixOfStr(dirRel.string(), path)) {
            continue;
        }
        result.insert(p.second);
    }
    return result;
}

std::unordered_set<std::string> Repo::findAllFileIdsExcept(
        const std::unordered_set<std::string>& ids,
        const boost::filesystem::path& dirRel) const {
    std::unordered_set<std::string> result;
    rocksdb::ReadOptions ro;
    auto db = _db->getDB();
    auto cfhTagFile = _db->getColumnFamilyHandle(Database::CF_TAG_FILE);
    auto cfhFile = _db->getColumnFamilyHandle(Database::CF_FILE);
    std::unique_ptr<rocksdb::Iterator> j(db->NewIterator(ro, cfhTagFile));
    for (j->SeekToFirst(); j->Valid(); j->Next()) {
        auto key = j->key().ToString();
        auto p = DBKey::split(key);
        if (ids.find(p.second) != ids.end()) {
            continue;
        }

        std::string path;
        db->Get(ro, cfhFile, DBKey::join(p.second, "path"), &path);
        if (!Repo::isPrefixOfStr(dirRel.string(), path)) {
            continue;
        }
        result.insert(p.second);
    }
    return result;
}

bool Repo::isPrefixOfStr(const std::string& prefix, const std::string& str) {
    auto res = std::mismatch(prefix.begin(), prefix.end(), str.begin());
    return res.first == prefix.end();
}

void Repo::createDirWatcherIfNeeded(const boost::filesystem::path& dirPath) {
    if (dirPath == _dbPath) {
        BOOST_LOG_TRIVIAL(debug) << "Skipping dir " << dirPath;

        return;
    }
    createDirWatcher(dirPath);
}

void Repo::createDirWatcher(const boost::filesystem::path& dirPath) {

    std::unique_ptr<Poco::DirectoryWatcher> watcher(new Poco::DirectoryWatcher(dirPath.string(),
            Poco::DirectoryWatcher::DW_FILTER_ENABLE_ALL, 2));
    watcher->itemAdded += Poco::delegate(this, &Repo::onFileAdded);
    watcher->itemRemoved += Poco::delegate(this, &Repo::onFileRemoved);
    watcher->itemModified += Poco::delegate(this, &Repo::onFileChanged);
    watcher->itemMovedFrom += Poco::delegate(this, &Repo::onFileMovedFrom);
    watcher->itemMovedTo += Poco::delegate(this, &Repo::onFileMovedTo);
    watcher->scanError += Poco::delegate(this, &Repo::onScanError);
    _watchers[dirPath.string()] = std::move(watcher);
    BOOST_LOG_TRIVIAL(debug) << "DirWatcher created for " << dirPath;
}

void Repo::destroyDirWatcherIfExists(const boost::filesystem::path& dirPath) {
    auto f = _watchers.find(dirPath.string());
    if (f != _watchers.end()) {

        _watchers.erase(f);
        BOOST_LOG_TRIVIAL(debug) << "DirWatcher destroyed for " << dirPath;
    }
    // else silently do nothing
}

void Repo::onFileAdded(const Poco::DirectoryWatcher::DirectoryEvent& event) {
    BOOST_LOG_TRIVIAL(debug) << "Added: " << event.item.path();
    if (event.item.exists() && event.item.isDirectory()) {

        createDirWatcher(event.item.path());
    }
}

void Repo::onFileRemoved(const Poco::DirectoryWatcher::DirectoryEvent& event) {

    const std::string& path = event.item.path();
    destroyDirWatcherIfExists(path);
    BOOST_LOG_TRIVIAL(debug) << "Removed: " << event.item.path();
}

void Repo::onFileChanged(const Poco::DirectoryWatcher::DirectoryEvent& event) {

    BOOST_LOG_TRIVIAL(debug) << "Changed: " << event.item.path();
}

void Repo::onFileMovedFrom(const Poco::DirectoryWatcher::DirectoryEvent& event) {

    BOOST_LOG_TRIVIAL(debug) << "MovedFrom: " << event.item.path();
}

void Repo::onFileMovedTo(const Poco::DirectoryWatcher::DirectoryEvent& event) {

    BOOST_LOG_TRIVIAL(debug) << "MovedTo: " << event.item.path();
}

void Repo::onScanError(const Poco::Exception& ex) {
    BOOST_LOG_TRIVIAL(debug) << "ScanError: " << ex.message();
}


