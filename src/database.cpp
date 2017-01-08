#include "database.h"
#include "first_delim_prefix_transform.h"
#include "reggata_exceptions.h"
#include "status_code.h"
#include <boost/filesystem.hpp>
#include <boost/format.hpp>

Database::Database(const std::string& dbPath, bool initIfNotExists) :
_dbPath(dbPath) {
    if (initIfNotExists && !boost::filesystem::exists(dbPath)) {
        auto ok = boost::filesystem::create_directories(dbPath);
        if (!ok) {
            throw StatusCodeException(StatusCode::SERVER_ERROR,
                    (boost::format("Could not create directory %1%") % dbPath).str());
        }
    }
    if (!boost::filesystem::exists(dbPath)) {
        throw StatusCodeException(StatusCode::CLIENT_ERROR,
                (boost::format("Database directory %1% doesn't exist") % dbPath).str());
    }
    rocksdb::DBOptions opts;
    opts.create_if_missing = initIfNotExists;
    opts.create_missing_column_families = true;
    auto families = columnFamilies();
    rocksdb::DB* db;
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    auto status = rocksdb::DB::Open(opts, _dbPath, families, &handles, &db);
    _db.reset(db);
    if (!status.ok()) {
        throw StatusCodeException(StatusCode::SERVER_ERROR,
                (boost::format("Could not open rocksdb database %1%, reason: %2%")
                % _dbPath % status.ToString()).str());
    }
    for (auto h : handles) {
        auto hPtr = std::unique_ptr<rocksdb::ColumnFamilyHandle>(h);
        _handles[h->GetName()] = std::move(hPtr);
    }
}

rocksdb::DB* Database::getDB() const {
    return _db.get();
}

rocksdb::ColumnFamilyHandle* Database::getColumnFamilyHandle(const std::string& columnFamilyName) const {
    return _handles.at(columnFamilyName).get();
}

const std::string Database::CF_DEFAULT = std::string("default");
const std::string Database::CF_FILE_PATH = std::string("file_path");
const std::string Database::CF_FILE = std::string("file");
const std::string Database::CF_FILE_TAG = std::string("file_tag");
const std::string Database::CF_TAG_FILE = std::string("tag_file");

std::vector<rocksdb::ColumnFamilyDescriptor> Database::columnFamilies() {
    std::vector<rocksdb::ColumnFamilyDescriptor> result;

    result.push_back(rocksdb::ColumnFamilyDescriptor(CF_DEFAULT, rocksdb::ColumnFamilyOptions()));

    rocksdb::ColumnFamilyOptions opts;
    opts.prefix_extractor = std::shared_ptr<rocksdb::SliceTransform>(new FirstDelimPrefixTransform());
    result.push_back(rocksdb::ColumnFamilyDescriptor(CF_FILE_PATH, opts));
    result.push_back(rocksdb::ColumnFamilyDescriptor(CF_FILE, opts));
    result.push_back(rocksdb::ColumnFamilyDescriptor(CF_FILE_TAG, opts));
    result.push_back(rocksdb::ColumnFamilyDescriptor(CF_TAG_FILE, opts));
    return result;
}