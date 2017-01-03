#include "client.h"
#include "common.h"
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

class OpenCloseRepoTest : public testing::Test {
public:
    fs::path _workDir;
    const int _port = 9100;
    std::unique_ptr<Application> _app;
    boost::thread _t;

    OpenCloseRepoTest() :
    _workDir("./test_data/open_close_repo_test"),
    _app(new Application(_port, false)),
    _t(&OpenCloseRepoTest::startReggataApp, this) {
    }

    void startReggataApp() {
        _app->start();
    }

    void SetUp() {
        fs::remove_all(_workDir);
    }

    void TearDown() {
        _app->stop();
        _t.join();
        fs::remove_all(_workDir);
    }

    ~OpenCloseRepoTest() {
        // cleanup any pending stuff, but no exceptions allowed
    }
};

TEST_F(OpenCloseRepoTest, InitRepoThenCloseThenOpen) {
    fs::path repoDir(_workDir / "repo");
    ASSERT_FALSE(fs::exists(repoDir));
    Client c(_port);
    json::json cmd = {
        {"id", "42"},
        {"cmd", "open_repo"},
        {"args",
            {
                {"root_dir", repoDir.c_str()},
                {"db_dir", (repoDir / ".reggata").c_str()},
                {"init_if_not_exists", true}
            }}
    };
    auto err = c.send(cmd.dump());
    ASSERT_EQ(nullptr, err) << "sendMsg failed, error: " << err;
    auto msg = c.recv();
    auto obj = json::json::parse(msg);
    ASSERT_EQ("42", obj["id"]);
    ASSERT_EQ(true, obj["ok"]);

    // TODO: close this repo

    // TODO: open this repo without flag init_if_not_exists
}

TEST_F(OpenCloseRepoTest, TryOpenNonExistentRepo) {
    fs::path repoDir(_workDir / "non_existent_repo");
    ASSERT_FALSE(fs::exists(repoDir));
    json::json cmd = {
        {"id", "123"},
        {"cmd", "open_repo"},
        {"args",
            {
                {"root_dir", repoDir.c_str()},
                {"db_dir", (repoDir / ".reggata").c_str()}
            }}
    };
    Client c(_port);
    auto err = c.send(cmd.dump());
    ASSERT_EQ(nullptr, err) << "sendMsg failed, error: " << err;
    auto msg = c.recv();
    auto obj = json::json::parse(msg);
    ASSERT_EQ("123", obj["id"]);
    ASSERT_EQ(false, obj["ok"]);
    ASSERT_EQ("Database directory ./test_data/open_close_repo_test/non_existent_repo/.reggata doesn't exist", obj["reason"]);

}

/* TODO: Add tests:
 * TryOpenRepoTwice
 * TryInitRepoInSubdirOfOpenedRepo
 * TryOpenNotARepo - when root_dir exists but it's not a reggata repo
 */
