#include "database.h"
#include "rocksdb_merge_operator.h"
#include "first_delim_prefix_transform.h"
#include "reggata_exceptions.h"
#include <boost/filesystem.hpp>

Database::Database(const std::string& dbPath, bool initIfNotExists = false) :
_dbPath(dbPath) {
    if (initIfNotExists && !boost::filesystem::exists(dbPath)) {
        auto ok = boost::filesystem::create_directories(dbPath);
        if (!ok) {
            throw ReggataException(std::string("Could not create directory ") + dbPath);
        }
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
        throw ReggataException(std::string("Could not open rocksdb database ") + _dbPath + ", reason: " + status.ToString());
    }
    for (auto h : handles) {
        _handles[h->GetName()] = h;
    }
}

std::vector<rocksdb::ColumnFamilyDescriptor> Database::columnFamilies() {
    std::vector<rocksdb::ColumnFamilyDescriptor> result;

    //result.push_back(rocksdb::ColumnFamilyDescriptor("default", rocksdb::ColumnFamilyOptions())); // TODO: maybe remove?..

    rocksdb::ColumnFamilyOptions opts1;
    opts1.merge_operator = std::shared_ptr<rocksdb::MergeOperator>(new UInt64AddOperator());
    result.push_back(rocksdb::ColumnFamilyDescriptor("counter", opts1));

    rocksdb::ColumnFamilyOptions opts2;
    opts2.prefix_extractor = std::shared_ptr<rocksdb::SliceTransform>(new FirstDelimPrefixTransform(":"));
    result.push_back(rocksdb::ColumnFamilyDescriptor("file_path", opts2));
    result.push_back(rocksdb::ColumnFamilyDescriptor("file", opts2));
    result.push_back(rocksdb::ColumnFamilyDescriptor("file_tag", opts2));
    result.push_back(rocksdb::ColumnFamilyDescriptor("tag_file", opts2));
    return result;
}