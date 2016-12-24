#pragma once
#include <rocksdb/db.h>
#include <memory>

class Database {
    std::string _dbPath;
    std::unique_ptr<rocksdb::DB> _db;
    std::map<std::string, rocksdb::ColumnFamilyHandle*> _handles; // NOTE: should be deleted before the *rocksdb::DB
    
public:
    Database(const std::string& dbPath, bool initIfNotExists);
    virtual ~Database() = default;
   
private:
    static std::vector<rocksdb::ColumnFamilyDescriptor> columnFamilies();
};

