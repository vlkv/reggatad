#include "client.h"
#include "common.h"
#include "status_code.h"
#include <application.h>
#include <nlohmann/json.hpp>
namespace json = nlohmann;
#include <gtest/gtest.h>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/timer.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
namespace fs = boost::filesystem;
#include <memory>
#include <iostream>
#include <sstream>

class AddTagsTest : public testing::Test {
public:
    const int _port = 9100;
    std::unique_ptr<Application> _app;
    boost::thread _t;
    fs::path _workDir;

    AddTagsTest() :
    _app(new Application(_port, false)),
    _t(&AddTagsTest::startReggataApp, this),
    _workDir("./test_data/add_tags_test") {
    }

    void startReggataApp() {
        _app->start();
    }

    void SetUp() {
        Client c(_port);
        fs::path dbDir(_workDir / ".reggata");
        fs::remove_all(dbDir);
        json::json cmd = {
            {"id", "123"},
            {"cmd", "open_repo"},
            {"args",
                {
                    {"root_dir", _workDir.c_str()},
                    {"db_dir", dbDir.c_str()},
                    {"init_if_not_exists", true}
                }}
        };
        auto err = c.send(cmd.dump());
        ASSERT_EQ(nullptr, err) << "sendMsg failed, error: " << err;
        auto msg = c.recv();
        auto obj = json::json::parse(msg);
        ASSERT_EQ("123", obj["id"]);
        ASSERT_EQ(StatusCode::OK, obj["code"]);
    }

    void TearDown() {
        _app->stop();
        _t.join();
    }

    ~AddTagsTest() {
        // cleanup any pending stuff, but no exceptions allowed
    }
};

TEST_F(AddTagsTest, Add3Tags) {
    Client c(_port);
    {
        json::json cmd = {
            {"id", "124"},
            {"cmd", "add_tags"},
            {"args",
                {
                    {"file", (_workDir / "dir" / "file").c_str()},
                    {"tags",
                        {"tag1", "tag2", "tag3"}}
                }}
        };
        auto err = c.send(cmd.dump());
        ASSERT_EQ(nullptr, err) << "sendMsg failed, error: " << err;
        auto msg = c.recv();
        auto obj = json::json::parse(msg);
        ASSERT_EQ("124", obj["id"]);
        ASSERT_EQ(StatusCode::OK, obj["code"]);
    }

    {// check the tags have been added
        json::json cmd = {
            {"id", "125"},
            {"cmd", "get_file_info"},
            {"args",
                {
                    {"file", (_workDir / "dir" / "file").c_str()},
                }}
        };
        auto err = c.send(cmd.dump());
        ASSERT_EQ(nullptr, err) << "sendMsg failed, error: " << err;
        auto msg = c.recv();
        auto obj = json::json::parse(msg);
        ASSERT_EQ("125", obj["id"]);
        ASSERT_EQ(StatusCode::OK, obj["code"]);

        std::vector<std::string> tags = obj["tags"];
        ASSERT_EQ(3, tags.size());
        ASSERT_NE(tags.end(), std::find(tags.begin(), tags.end(), "tag1"));
        ASSERT_NE(tags.end(), std::find(tags.begin(), tags.end(), "tag2"));
        ASSERT_NE(tags.end(), std::find(tags.begin(), tags.end(), "tag3"));
    }
}

TEST_F(AddTagsTest, TryAddTagsToNonexistentFile) {
    Client c(_port);
    json::json cmd2 = {
        {"id", "124"},
        {"cmd", "add_tags"},
        {"args",
            {
                {"file", (_workDir / "nonexistent_file").c_str()},
                {"tags",
                    {"tag1", "tag2", "tag3"}}
            }}
    };
    auto err2 = c.send(cmd2.dump());
    ASSERT_EQ(nullptr, err2) << "sendMsg failed, error: " << err2;
    auto msg2 = c.recv();
    auto obj2 = json::json::parse(msg2);
    ASSERT_EQ("124", obj2["id"]);
    ASSERT_EQ(StatusCode::CLIENT_ERROR, obj2["code"]);
    ASSERT_EQ("Could not add tags, reason: file \"./test_data/add_tags_test/nonexistent_file\" does not exists", obj2["reason"]);
}