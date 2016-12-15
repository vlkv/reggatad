#include "application.h"
#include "client.h"
#include "common.h"
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

class OpenCloseRepoTest : public testing::Test {
public:
    const int PORT = 9100;
    std::unique_ptr<Application> _app;
    boost::thread _t;

    OpenCloseRepoTest() :
    _app(new Application(PORT, false)),
    _t(&OpenCloseRepoTest::startReggataApp, this) {
        boost::this_thread::sleep(boost::posix_time::seconds(1));
    }

    void startReggataApp() {
        _app->start();
    }

    void SetUp() {
    }

    void TearDown() {
    }

    ~OpenCloseRepoTest() {
        // cleanup any pending stuff, but no exceptions allowed
        try {
            _app->stop();
            _t.join();
        } catch (const std::exception& ex) {
            std::cout << "~TestFixture: std::exception, " << ex.what();
        } catch (...) {
            std::cout << "~TestFixture: unknown exception";
        }
    }
};

TEST_F(OpenCloseRepoTest, StartStop) {
    std::cout << "do nothing" << std::endl;
}

TEST_F(OpenCloseRepoTest, OpenRepo1) {
    fs::path REPO_ROOT("./test_data/repo1");
    Client c(PORT);
    json::json cmd = {
        {"id", "123"},
        {"cmd", "open_repo"},
        {"args",
            {
                {"root_dir", REPO_ROOT.c_str()},
                {"db_dir", (REPO_ROOT / ".reggata").c_str()}
            }}
    };
    auto err = c.send(cmd.dump());
    ASSERT_EQ(nullptr, err) << "sendMsg failed, error: " << err;
    auto msg = c.recv();
    auto obj = json::json::parse(msg);
    ASSERT_EQ("123", obj["id"]);
    ASSERT_EQ(true, obj["ok"]);
}

TEST_F(OpenCloseRepoTest, InitAndOpenNonExistentRepo) {
    fs::path REPO_ROOT("./test_data/non_existent_repo");
    fs::remove_all(REPO_ROOT);
    Client c(PORT);
    json::json cmd = {
        {"id", "123"},
        {"cmd", "open_repo"},
        {"args",
            {
                {"root_dir", REPO_ROOT.c_str()},
                {"db_dir", (REPO_ROOT / ".reggata").c_str()},
                {"init_if_not_exists", true}
            }}
    };
    auto err = c.send(cmd.dump());
    ASSERT_EQ(nullptr, err) << "sendMsg failed, error: " << err;
    auto msg = c.recv();
    auto obj = json::json::parse(msg);
    ASSERT_EQ("123", obj["id"]);
    ASSERT_EQ(true, obj["ok"]);
}

TEST_F(OpenCloseRepoTest, OpenNonExistentRepoAndFail) {
    Client c(PORT);
    {
        fs::path REPO_ROOT("./test_data/non_existent_repo");
        fs::remove_all(REPO_ROOT);
        json::json cmd = {
            {"id", "123"},
            {"cmd", "open_repo"},
            {"args",
                {
                    {"root_dir", REPO_ROOT.c_str()},
                    {"db_dir", (REPO_ROOT / ".reggata").c_str()}
                }}
        };
        auto err = c.send(cmd.dump());
        ASSERT_EQ(nullptr, err) << "sendMsg failed, error: " << err;
        auto msg = c.recv();
        auto obj = json::json::parse(msg);
        ASSERT_EQ("123", obj["id"]);
        ASSERT_EQ(false, obj["ok"]);
        ASSERT_EQ("Could not open rocksdb database at ./test_data/non_existent_repo/.reggata",
                obj["reason"]);
    }
}

// TODO: move this test to separate cpp file

TEST_F(OpenCloseRepoTest, OpenRepo1AddTag) {
    fs::path REPO_ROOT("./test_data/repo1");
    Client c(PORT);
    json::json cmd = {
        {"id", "123"},
        {"cmd", "open_repo"},
        {"args",
            {
                {"root_dir", REPO_ROOT.c_str()},
                {"db_dir", (REPO_ROOT / ".reggata").c_str()}
            }}
    };
    auto err = c.send(cmd.dump());
    ASSERT_EQ(nullptr, err) << "sendMsg failed, error: " << err;
    auto msg = c.recv();
    auto obj = json::json::parse(msg);
    ASSERT_EQ("123", obj["id"]);
    ASSERT_EQ(true, obj["ok"]);

    json::json cmd2 = {
        {"id", "124"},
        {"cmd", "add_tags"},
        {"args",
            {
                {"file", (REPO_ROOT / "dir" / "file").c_str()},
                {"tags",
                    {"tag1", "tag2", "tag3"}}
            }}
    };
    auto err2 = c.send(cmd2.dump());
    ASSERT_EQ(nullptr, err2) << "sendMsg failed, error: " << err2;
    auto msg2 = c.recv();
    auto obj2 = json::json::parse(msg2);
    ASSERT_EQ("124", obj["id"]);
    ASSERT_EQ(true, obj["ok"]);
}