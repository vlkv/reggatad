#include "client.h"
#include "common.h"
#include <status_code.h>
#include <application.h>
#include <nlohmann/json.hpp>
#include <gtest/gtest.h>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/timer.hpp>
#include <memory>
#include <iostream>
#include <sstream>

class BadCmdTest : public testing::Test {
public:
    const int _port = 9100;
    std::unique_ptr<Application> _app;
    boost::thread _t;

    BadCmdTest() :
    _app(new Application(_port, 0)),
    _t(&BadCmdTest::startReggataApp, this) {
    }

    void startReggataApp() {
        _app->start();
    }

    void SetUp() {
    }

    void TearDown() {
        _app->stop();
        _t.join();

    }

    ~BadCmdTest() {
        // cleanup any pending stuff, but no exceptions allowed
    }
};

TEST_F(BadCmdTest, InvalidJson) {
    Client c(_port);
    c.send("This is NOT a JSON string");
    auto msg = c.recv();
    auto obj = nlohmann::json::parse(msg);
    ASSERT_EQ(StatusCode::CLIENT_ERROR, obj["code"]);
    ASSERT_EQ("parse error - unexpected 'T'", obj["msg"]);
}

TEST_F(BadCmdTest, BlaBlaBlaJsonMessage) {
    nlohmann::json cmd = {
        {"id", "2"},
        {"bla", "bla bla"}
    };
    Client c(_port);
    c.send(cmd.dump());
    auto msg = c.recv();
    auto obj = nlohmann::json::parse(msg);
    ASSERT_EQ("2", obj["id"]);
    ASSERT_EQ(StatusCode::CLIENT_ERROR, obj["code"]);
    ASSERT_EQ("key 'cmd' not found", obj["msg"]);
}

TEST_F(BadCmdTest, IncompleteCmdNoArgs) {
    nlohmann::json cmd = {
        {"id", "1"},
        {"cmd", "open_repo"}
    };
    Client c(_port);
    c.send(cmd.dump());
    auto msg = c.recv();
    auto obj = nlohmann::json::parse(msg);
    ASSERT_EQ("1", obj["id"]);
    ASSERT_EQ(StatusCode::CLIENT_ERROR, obj["code"]);
    ASSERT_EQ("key 'root_dir' not found", obj["msg"]);
}

TEST_F(BadCmdTest, IncompleteCmdNoArgs2) {
    nlohmann::json cmd = {
        {"id", "1"},
        {"cmd", "open_repo"},
        {"args",
            nlohmann::json::object()}
    };
    Client c(_port);
    c.send(cmd.dump());
    auto msg = c.recv();
    auto obj = nlohmann::json::parse(msg);
    ASSERT_EQ("1", obj["id"]);
    ASSERT_EQ(StatusCode::CLIENT_ERROR, obj["code"]);
    ASSERT_EQ("key 'root_dir' not found", obj["msg"]);
}

TEST_F(BadCmdTest, IncompleteCmdNoId) {
    nlohmann::json cmd = {
        {"cmd", "open_repo"},
        {"args",
            {
                {"root_dir", "/tmp/repo"},
                {"db_dir", "/tmp/repo/.reggata"}
            }}
    };
    Client c(_port);
    c.send(cmd.dump());
    auto msg = c.recv();
    auto obj = nlohmann::json::parse(msg);
    ASSERT_EQ(StatusCode::CLIENT_ERROR, obj["code"]);
    ASSERT_EQ("key 'id' not found", obj["msg"]);
}

TEST_F(BadCmdTest, IncompleteCmdNoMandatoryArg) {
    nlohmann::json cmd = {
        {"id", "1"},
        {"cmd", "open_repo"},
        {"args",
            {
                {"db_dir", "/tmp/repo/.reggata"}
            }}
    };
    Client c(_port);
    c.send(cmd.dump());
    auto msg = c.recv();
    auto obj = nlohmann::json::parse(msg);
    ASSERT_EQ(StatusCode::CLIENT_ERROR, obj["code"]);
    ASSERT_EQ("key 'root_dir' not found", obj["msg"]);
}

TEST_F(BadCmdTest, InvalidCmdArgType) {
    nlohmann::json cmd = {
        {"id", "1"},
        {"cmd", "open_repo"},
        {"args",
            {
                {"root_dir", "/tmp/repo"},
                {"db_dir", 42}
            }}
    };
    Client c(_port);
    c.send(cmd.dump());
    auto msg = c.recv();
    auto obj = nlohmann::json::parse(msg);
    ASSERT_EQ(StatusCode::CLIENT_ERROR, obj["code"]);
    ASSERT_EQ("type must be string, but is number", obj["msg"]);
}
