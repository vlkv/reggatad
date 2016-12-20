#include "database.h"
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
    
    // TODO: Use column families

    rocksdb::Options options;
    options.create_if_missing = initIfNotExists;
    rocksdb::DB* db;
    rocksdb::Status status = rocksdb::DB::Open(options, _dbPath, &db);
    _db.reset(db);
    if (!status.ok()) {
        throw ReggataException(std::string("Could not open rocksdb database ") + _dbPath + ", reason: " + status.ToString());
    }
}

