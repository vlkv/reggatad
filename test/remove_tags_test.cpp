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
#include <memory>
#include <iostream>
#include <sstream>

class RemoveTagsTest : public testing::Test {
public:
    const int _port = 9100;
    std::unique_ptr<Application> _app;
    boost::thread _t;
    boost::filesystem::path _workDir;

    RemoveTagsTest() :
    _app(new Application(_port, 0)),
    _t(&RemoveTagsTest::startReggataApp, this),
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

        auto r2 = c.addTags(_workDir / "dir" / "file", std::vector<std::string>{"tag1", "tag2", "tag3"});
        ASSERT_EQ(StatusCode::OK, r2["code"]);
    }

    void TearDown() {
        _app->stop();
        _t.join();
    }

    ~RemoveTagsTest() {
        // cleanup any pending stuff, but no exceptions allowed
    }
};

TEST_F(RemoveTagsTest, RemoveTags) {
    Client c(_port);
    {
        nlohmann::json cmd = {
            {"id", "2"},
            {"cmd", "remove_tags"},
            {"args",
                {
                    {"file", (_workDir / "dir" / "file").c_str()},
                    {"tags",
                        {"tag1", "tag3", "NonExistentTag"}}
                }}
        };
        c.send(cmd.dump());
        auto msg = c.recv();
        auto obj = nlohmann::json::parse(msg);
        ASSERT_EQ("2", obj["id"]);
        ASSERT_EQ(StatusCode::OK, obj["code"]); // NOTE: removal of "NonExistentTag" doesn't hurt
    }

    {// check the tags have been removed
        nlohmann::json cmd = {
            {"id", "3"},
            {"cmd", "get_file_info"},
            {"args",
                {
                    {"file", (_workDir / "dir" / "file").c_str()},
                }}
        };
        c.send(cmd.dump());
        auto msg = c.recv();
        auto obj = nlohmann::json::parse(msg);
        ASSERT_EQ("3", obj["id"]);
        ASSERT_EQ(StatusCode::OK, obj["code"]);
        auto objData = obj["data"];
        ASSERT_EQ(4, objData["size"]);
        ASSERT_EQ("dir/file", objData["path"]) << "Path must be relative to repo root dir";

        std::vector<std::string> tags = objData["tags"];
        ASSERT_EQ(1, tags.size());
        ASSERT_NE(tags.end(), std::find(tags.begin(), tags.end(), "tag2"));
    }
}

TEST_F(RemoveTagsTest, TryRemoveTagsToNonexistentFile) {
    Client c(_port);
    nlohmann::json cmd2 = {
        {"id", "124"},
        {"cmd", "remove_tags"},
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
    ASSERT_EQ("Could not remove tags, reason: file \"/home/vitvlkv/dev/reggatad/test_data/repo/nonexistent_file\" does not exists", obj2["msg"]);
}