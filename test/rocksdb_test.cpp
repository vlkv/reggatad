#include "common.h"
#include "rocksdb_merge_operator.h"
#include <reggata_exceptions.h>

#include <rocksdb/db.h>
#include <json.hpp>
namespace json = nlohmann;

#include <gtest/gtest.h>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
namespace fs = boost::filesystem;

#include <memory>
#include <iostream>
#include <sstream>

class RocksDBTest : public testing::Test {
public:
    fs::path DB_PATH;
    

    RocksDBTest() :
    DB_PATH("./test_data/rocksdb_test") {
    }

    void SetUp() {
                
    }

    void TearDown() {
    }

    ~RocksDBTest() {
        // cleanup any pending stuff, but no exceptions allowed
    }
};


TEST_F(RocksDBTest, BasicOperations) {
    rocksdb::Options options;
    options.create_if_missing = true;
    options.merge_operator = std::shared_ptr<rocksdb::MergeOperator>(new UInt64AddOperator());
    rocksdb::DB* dbRaw;
    rocksdb::Status status = rocksdb::DB::Open(options, DB_PATH.string(), &dbRaw);
    std::unique_ptr<rocksdb::DB> db(dbRaw);
    ASSERT_EQ(true, status.ok()) << status.ToString();

    rocksdb::WriteOptions wo;
    wo.sync=true;
    db->Put(wo, "tag1:1", "a");
    db->Put(wo, "tag1:2", "b");
    db->Put(wo, "tag2:2", "c");
    
    rocksdb::ReadOptions ro;
    std::string val;
    rocksdb::Status s;
    s = db->Get(ro, "tag1:1", &val);
    ASSERT_EQ(true, s.ok()) << s.ToString();
    ASSERT_EQ("a", val);
    
    s = db->Get(ro, "non_existent_key", &val);
    ASSERT_EQ(false, s.ok()) << s.ToString();
    
    s = db->Delete(wo, "next_id");
    ASSERT_EQ(true, s.ok()) << s.ToString();
    s = db->Merge(wo, "next_id", "1");
    ASSERT_EQ(true, s.ok()) << s.ToString();
    s = db->Get(ro, "next_id", &val);
    ASSERT_EQ("1", val) << s.ToString();
    s = db->Merge(wo, "next_id", "1");
    ASSERT_EQ(true, s.ok()) << s.ToString();
    s = db->Get(ro, "next_id", &val);
    ASSERT_EQ("2", val) << s.ToString();
}
