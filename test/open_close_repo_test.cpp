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

class OpenCloseRepoTest : public testing::Test {
public:
    boost::filesystem::path _workDir;
    const int _port = 9100;
    std::unique_ptr<Application> _app;
    boost::thread _t;

    OpenCloseRepoTest() :
    _workDir("./test_data/open_close_repo_test"),
    _app(new Application(_port, 0)),
    _t(&OpenCloseRepoTest::startReggataApp, this) {
    }

    void startReggataApp() {
        _app->start();
    }

    void SetUp() {
        boost::filesystem::create_directories(_workDir);
        _workDir = boost::filesystem::canonical(_workDir);
    }

    void TearDown() {
        _app->stop();
        _t.join();
        boost::filesystem::remove_all(_workDir);
    }

    ~OpenCloseRepoTest() {
        // cleanup any pending stuff, but no exceptions allowed
    }
};

TEST_F(OpenCloseRepoTest, InitRepoThenCloseThenOpen) {
    boost::filesystem::path repoDir(_workDir / "repo");
    boost::filesystem::path dbDir(repoDir / ".reggata");
    ASSERT_FALSE(boost::filesystem::exists(repoDir));
    Client c(_port);
    auto obj = c.initRepo(repoDir, dbDir);
    ASSERT_EQ(StatusCode::OK, obj["code"]);

    auto obj2 = c.getReposInfo();
    ASSERT_EQ(1, obj2["data"].size());
    auto ri = RepoInfo::fromJson(obj2["data"][0]);
    ASSERT_EQ(repoDir.string(), ri._rootDir);
    ASSERT_EQ(dbDir.string(), ri._dbDir);

    auto obj3 = c.closeRepo(repoDir);
    ASSERT_EQ(StatusCode::OK, obj3["code"]);

    auto obj4 = c.getReposInfo();
    ASSERT_EQ(0, obj4["data"].size());

    // TODO: open this repo without flag init_if_not_exists
}

TEST_F(OpenCloseRepoTest, TryOpenNonExistentRepo) {
    boost::filesystem::path repoDir(_workDir / "non_existent_repo");
    ASSERT_FALSE(boost::filesystem::exists(repoDir));
    nlohmann::json cmd = {
        {"id", "123"},
        {"cmd", "open_repo"},
        {"args",
            {
                {"root_dir", repoDir.c_str()},
                {"db_dir", (repoDir / ".reggata").c_str()}
            }}
    };
    Client c(_port);
    c.send(cmd.dump());
    auto msg = c.recv();
    auto obj = nlohmann::json::parse(msg);
    ASSERT_EQ("123", obj["id"]);
    ASSERT_EQ(StatusCode::CLIENT_ERROR, obj["code"]);
    ASSERT_EQ("Database directory \"/home/vitvlkv/dev/reggatad/test_data/open_close_repo_test/non_existent_repo/.reggata\" doesn't exist", obj["msg"]);
}

/* TODO: Add tests:
 * TryOpenRepoTwice
 * TryInitRepoInSubdirOfOpenedRepo
 * TryOpenNotARepo - when root_dir exists but it's not a reggata repo
 */
