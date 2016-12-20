#pragma once
#include <rocksdb/db.h>
#include <memory>

class Database {
    std::string _dbPath;
    std::unique_ptr<rocksdb::DB> _db;
    
    std::vector<rocksdb::ColumnFamilyHandle*> _handles;
    
public:
    Database(const std::string& dbPath, bool initIfNotExists);
    virtual ~Database() = default;
   
private:
    static std::vector<rocksdb::ColumnFamilyDescriptor> columnFamilies();
};

