#include "common.h"
#include <first_delim_prefix_transform.h>
#include <reggata_exceptions.h>

#include <rocksdb/db.h>
#include <rocksdb/slice_transform.h>
#include <nlohmann/json.hpp>

#include <gtest/gtest.h>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <memory>
#include <iostream>
#include <sstream>

class RocksDBTest : public testing::Test {
public:
    boost::filesystem::path DB_PATH;

    RocksDBTest() :
    DB_PATH("./test_data/rocksdb_test") {
    }

    void SetUp() {
        boost::system::error_code ec;
        boost::filesystem::remove_all(DB_PATH, ec);
        ASSERT_EQ(boost::system::errc::success, ec.value()) << "Could not remove_all in " << DB_PATH;
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
    options.prefix_extractor = std::shared_ptr<rocksdb::SliceTransform>(new FirstDelimPrefixTransform());
    rocksdb::DB* dbRaw;
    rocksdb::Status status = rocksdb::DB::Open(options, DB_PATH.string(), &dbRaw);
    std::unique_ptr<rocksdb::DB> db(dbRaw);
    ASSERT_EQ(true, status.ok()) << status.ToString();

    ASSERT_EQ("tag1", options.prefix_extractor->Transform("tag1:1").ToString());
    ASSERT_EQ("tag1", options.prefix_extractor->Transform("tag1:").ToString());
    //ASSERT_EQ("", options.prefix_extractor->Transform("tag1").ToString());


    rocksdb::WriteOptions wo;
    wo.sync = true;
    db->Put(wo, "blabla:1", "bla1");
    db->Put(wo, "tag1:1", "a");
    db->Put(wo, "tag1:2", "b");
    db->Put(wo, "taggg:2", "c");
    db->Put(wo, "blabla:2", "bla2");

    rocksdb::ReadOptions ro;
    ro.prefix_same_as_start = true;
    std::string val;
    rocksdb::Status s;
    s = db->Get(ro, "tag1:1", &val);
    ASSERT_EQ(true, s.ok()) << s.ToString();
    ASSERT_EQ("a", val);

    std::unique_ptr<rocksdb::Iterator> j(db->NewIterator(ro));
    for (j->Seek("blabla:"); j->Valid(); j->Next()) {
        std::cout << j->key().ToString() << " = " << j->value().ToString() << std::endl;
    }

    std::unique_ptr<rocksdb::Iterator> i(db->NewIterator(ro));
    i->Seek("tag1:");
    ASSERT_TRUE(i->Valid());
    ASSERT_EQ("tag1:1", i->key());
    i->Next();
    ASSERT_TRUE(i->Valid());
    ASSERT_EQ("tag1:2", i->key());
    i->Next();
    ASSERT_FALSE(i->Valid());


    s = db->Get(ro, "non_existent_key", &val);
    ASSERT_FALSE(s.ok()) << s.ToString();
    ASSERT_TRUE(s.IsNotFound()) << s.ToString();
}
