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

class AddTagsTest : public testing::Test {
public:
    const int PORT = 9100;
    std::unique_ptr<Application> _app;
    boost::thread _t;
    std::unique_ptr<Client> _c;
    fs::path REPO_ROOT;

    AddTagsTest() :
    _app(new Application(PORT, false)),
    _t(&AddTagsTest::startReggataApp, this),
    _c(new Client(PORT)),
    REPO_ROOT("./test_data/repo1") {
        boost::this_thread::sleep(boost::posix_time::seconds(1));
    }

    void startReggataApp() {
        _app->start();
    }

    void SetUp() {
        json::json cmd = {
            {"id", "123"},
            {"cmd", "open_repo"},
            {"args",
                {
                    {"root_dir", REPO_ROOT.c_str()},
                    {"db_dir", (REPO_ROOT / ".reggata").c_str()}
                }}
        };
        auto err = _c->send(cmd.dump());
        ASSERT_EQ(nullptr, err) << "sendMsg failed, error: " << err;
        auto msg = _c->recv();
        auto obj = json::json::parse(msg);
        ASSERT_EQ("123", obj["id"]);
        ASSERT_EQ(true, obj["ok"]);
    }

    void TearDown() {
        _app->stop();
        _t.join();
    }

    ~AddTagsTest() {
        // cleanup any pending stuff, but no exceptions allowed
    }
};


TEST_F(AddTagsTest, OpenRepo1AddTag) {
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
    auto err2 = _c->send(cmd2.dump());
    ASSERT_EQ(nullptr, err2) << "sendMsg failed, error: " << err2;
    auto msg2 = _c->recv();
    auto obj2 = json::json::parse(msg2);
    ASSERT_EQ("124", obj2["id"]);
    ASSERT_EQ(true, obj2["ok"]);
}