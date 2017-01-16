#pragma once
#include <rocksdb/db.h>
#include <boost/filesystem.hpp>
#include <memory>

class Database {
    boost::filesystem::path _dbPath;
    std::unique_ptr<rocksdb::DB> _db;
    std::map<std::string, std::unique_ptr<rocksdb::ColumnFamilyHandle>> _handles; // NOTE: should be deleted before the *rocksdb::DB
    
public:
    Database(const boost::filesystem::path& dbPath, bool initIfNotExists = false);
    virtual ~Database() = default;
   
    rocksdb::DB* getDB() const;
    rocksdb::ColumnFamilyHandle* getColumnFamilyHandle(const std::string& columnFamilyName) const;
    
    static const std::string CF_DEFAULT;
    static const std::string CF_FILE_PATH;
    static const std::string CF_FILE;
    static const std::string CF_FILE_TAG;
    static const std::string CF_TAG_FILE;
    
private:
    static std::vector<rocksdb::ColumnFamilyDescriptor> columnFamilies();
};

