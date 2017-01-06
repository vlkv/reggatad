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
    _app(new Application(_port, false)),
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
    ASSERT_EQ("parse error - unexpected 'T'", obj["reason"]);
}

TEST_F(BadCmdTest, BlaBlaBlaJsonMessage) {
    nlohmann::json cmd = {
        {"bla", "bla bla"}
    };
    Client c(_port);
    c.send(cmd.dump());
    auto msg = c.recv();
    auto obj = nlohmann::json::parse(msg);
    ASSERT_EQ(StatusCode::CLIENT_ERROR, obj["code"]);
    ASSERT_EQ("Field 'cmd' is missing or empty", obj["reason"]);
}

TEST_F(BadCmdTest, DISABLED_IncompleteCmdNoArgs) { //TODO: undisable the test
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
    ASSERT_EQ("TODO", obj["reason"]);
}

TEST_F(BadCmdTest, DISABLED_IncompleteCmdNoArgs2) { //TODO: undisable the test
    nlohmann::json cmd = {
        {"id", "1"},
        {"cmd", "open_repo"},
        {"args",
            {}}
    };
    Client c(_port);
    c.send(cmd.dump());
    auto msg = c.recv();
    auto obj = nlohmann::json::parse(msg);
    ASSERT_EQ("1", obj["id"]);
    ASSERT_EQ(StatusCode::CLIENT_ERROR, obj["code"]);
    ASSERT_EQ("TODO", obj["reason"]);
}

TEST_F(BadCmdTest, DISABLED_IncompleteCmdNoId) { //TODO: undisable the test
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
    ASSERT_EQ("TODO", obj["reason"]);
}

TEST_F(BadCmdTest, DISABLED_IncompleteCmdNoMandatoryArg) { //TODO: undisable the test
    nlohmann::json cmd = {
        {"id", "1"},
        {"cmd", "open_repo"},
        {"args",
            {
                // NOTE mandatory arg 'root_dir' is missing
                {"db_dir", "/tmp/repo/.reggata"}
            }}
    };
    Client c(_port);
    c.send(cmd.dump());
    auto msg = c.recv();
    auto obj = nlohmann::json::parse(msg);
    ASSERT_EQ(StatusCode::CLIENT_ERROR, obj["code"]);
    ASSERT_EQ("TODO", obj["reason"]);
}

TEST_F(BadCmdTest, DISABLED_InvalidCmdArgType) { //TODO: undisable the test
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
    ASSERT_EQ("TODO", obj["reason"]);
}