#pragma once
#include "safe_queue.h"
#include "database.h"
#include "file_info.h"
#include <cmd/cmd_repo.h>
#include <Poco/DirectoryWatcher.h>
#include <Poco/Delegate.h>
#include <boost/log/trivial.hpp>
#include <boost/filesystem.hpp>
#include <boost/uuid/random_generator.hpp>
#include <string>
#include <set>
#include <map>
#include <memory>

class Repo {
    boost::filesystem::path _rootPath;
    boost::filesystem::path _dbPath;
    std::unique_ptr<Database> _db;
    boost::uuids::random_generator _uuidGenerator;
    SafeQueue<std::unique_ptr<CmdRepo>> _queue;
    volatile bool _stopCalled = false;
    std::map<const std::string, std::unique_ptr<Poco::DirectoryWatcher>> _watchers;
    boost::thread _thread;
    
public:
    Repo(const boost::filesystem::path& rootAbsPath, const boost::filesystem::path& dbPath, bool initIfNotExists = false);
    virtual ~Repo() = default;
    
    void stop();
    
    boost::filesystem::path rootPath() const;
    boost::filesystem::path dbPath() const;
    void enqueueCmd(std::unique_ptr<CmdRepo> cmd);
    
    void addTags(const boost::filesystem::path& fileAbs, const std::vector<std::string>& tags);
    
    void removeTags(const boost::filesystem::path& fileAbs, const std::vector<std::string>& tags);
    
    FileInfo getFileInfo(const boost::filesystem::path& fileAbs) const;
    FileInfo getFileInfoById(const std::string& fileId) const;
    std::vector<FileInfo> getFileInfos(const std::set<std::string>& fileIds) const;
    
    std::set<std::string> findFileIds(
        const std::string& tag, 
        const boost::filesystem::path& dirRel) const;
    
    std::set<std::string> findAllFileIdsExcept(
        const std::set<std::string>& ids, 
        const boost::filesystem::path& dirRel) const;
    
    boost::filesystem::path makeRelativePath(const boost::filesystem::path& abs) const;
    
private:
    std::string getOrCreateFileId(const boost::filesystem::path& fileRel);
    bool getFileId(const boost::filesystem::path& fileRel, std::string* fileId) const;
    std::string createFileId(const boost::filesystem::path& fileRel);
    void addTag(const std::string& fileId, const std::string& tag);
    void removeTag(const std::string& fileId, const std::string& tag);
    static bool isPrefixOfStr(const std::string& prefix, const std::string& str);

private:
    void run();
    
    void createDirWatcherIfNeeded(const boost::filesystem::path& dirPath);
    void createDirWatcher(const boost::filesystem::path& dirPath);
    void destroyDirWatcherIfExists(const boost::filesystem::path& dirPath);

    void onFileAdded(const Poco::DirectoryWatcher::DirectoryEvent& event);
    void onFileRemoved(const Poco::DirectoryWatcher::DirectoryEvent& event);
    void onFileChanged(const Poco::DirectoryWatcher::DirectoryEvent& event);
    void onFileMovedFrom(const Poco::DirectoryWatcher::DirectoryEvent& event);
    void onFileMovedTo(const Poco::DirectoryWatcher::DirectoryEvent& event);
    void onScanError(const Poco::Exception& ex);
};
