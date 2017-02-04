#include "client.h"
#include "common.h"
#include "status_code.h"
#include <application.h>
#include <nlohmann/json.hpp>
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
    boost::filesystem::path _workDir;

    AddTagsTest() :
    _app(new Application(_port, 0)),
    _t(&AddTagsTest::startReggataApp, this),
    _workDir(boost::filesystem::canonical("./test_data/repo")) {
    }

    void startReggataApp() {
        _app->start();
    }

    void SetUp() {
        Client c(_port);
        boost::filesystem::path dbDir(_workDir / ".reggata");
        boost::filesystem::remove_all(dbDir);
        auto r1 = c.initRepo(_workDir, dbDir);
        ASSERT_EQ(StatusCode::OK, r1["code"]);
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
        nlohmann::json cmd = {
            {"id", "124"},
            {"cmd", "add_tags"},
            {"args",
                {
                    {"file", (_workDir / "dir" / "file").c_str()},
                    {"tags",
                        {"tag1", "tag2", "tag3"}}
                }}
        };
        c.send(cmd.dump());
        auto msg = c.recv();
        auto obj = nlohmann::json::parse(msg);
        ASSERT_EQ("124", obj["id"]);
        ASSERT_EQ(StatusCode::OK, obj["code"]);
    }

    {// check the tags have been added
        nlohmann::json cmd = {
            {"id", "125"},
            {"cmd", "get_file_info"},
            {"args",
                {
                    {"file", (_workDir / "dir" / "file").c_str()},
                }}
        };
        c.send(cmd.dump());
        auto msg = c.recv();
        auto obj = nlohmann::json::parse(msg);
        ASSERT_EQ("125", obj["id"]);
        ASSERT_EQ(StatusCode::OK, obj["code"]);
        auto objData = obj["data"];
        ASSERT_EQ(4, objData["size"]);
        ASSERT_EQ("dir/file", objData["path"]) << "Path must be relative to repo root dir";

        std::vector<std::string> tags = objData["tags"];
        ASSERT_EQ(3, tags.size());
        ASSERT_NE(tags.end(), std::find(tags.begin(), tags.end(), "tag1"));
        ASSERT_NE(tags.end(), std::find(tags.begin(), tags.end(), "tag2"));
        ASSERT_NE(tags.end(), std::find(tags.begin(), tags.end(), "tag3"));
    }
}

TEST_F(AddTagsTest, AddSameTagsMultipleTimes) {
    Client c(_port);
    {
        nlohmann::json cmd = {
            {"id", "1"},
            {"cmd", "add_tags"},
            {"args",
                {
                    {"file", (_workDir / "dir" / "file").c_str()},
                    {"tags",
                        {"A", "B", "C", "C"}}
                }}
        };
        c.send(cmd.dump());
        auto msg = c.recv();
        auto obj = nlohmann::json::parse(msg);
        ASSERT_EQ("1", obj["id"]);
        ASSERT_EQ(StatusCode::OK, obj["code"]);
    }

    {
        nlohmann::json cmd = {
            {"id", "2"},
            {"cmd", "add_tags"},
            {"args",
                {
                    {"file", (_workDir / "dir" / "file").c_str()},
                    {"tags",
                        {"B", "C"}}
                }}
        };
        c.send(cmd.dump());
        auto msg = c.recv();
        auto obj = nlohmann::json::parse(msg);
        ASSERT_EQ("2", obj["id"]);
        ASSERT_EQ(StatusCode::OK, obj["code"]);
    }

    {// check the tags have been added
        auto obj = c.getFileInfo(_workDir / "dir" / "file");
        ASSERT_EQ(StatusCode::OK, obj["code"]);
        auto objData = obj["data"];
        ASSERT_EQ(4, objData["size"]);
        ASSERT_EQ("dir/file", objData["path"]) << "Path must be relative to repo root dir";

        std::vector<std::string> tags = objData["tags"];
        ASSERT_EQ(3, tags.size());
        ASSERT_NE(tags.end(), std::find(tags.begin(), tags.end(), "A"));
        ASSERT_NE(tags.end(), std::find(tags.begin(), tags.end(), "B"));
        ASSERT_NE(tags.end(), std::find(tags.begin(), tags.end(), "C"));
    }
}

TEST_F(AddTagsTest, TryAddTagsToNonexistentFile) {
    Client c(_port);
    nlohmann::json cmd2 = {
        {"id", "124"},
        {"cmd", "add_tags"},
        {"args",
            {
                {"file", (_workDir / "nonexistent_file").c_str()},
                {"tags",
                    {"tag1", "tag2", "tag3"}}
            }}
    };
    c.send(cmd2.dump());
    auto msg2 = c.recv();
    auto obj2 = nlohmann::json::parse(msg2);
    ASSERT_EQ("124", obj2["id"]);
    ASSERT_EQ(StatusCode::CLIENT_ERROR, obj2["code"]);
    ASSERT_EQ("Could not add tags, reason: file \"/home/vitvlkv/dev/reggatad/test_data/repo/nonexistent_file\" does not exists", obj2["msg"]);
}