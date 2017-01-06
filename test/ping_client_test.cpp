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

class PingClientTest : public testing::Test {
public:
    const int _port = 9100;
    std::unique_ptr<Application> _app;
    boost::thread _t;

    PingClientTest() :
    _app(new Application(_port, true)),
    _t(&PingClientTest::startReggataApp, this) {
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

    ~PingClientTest() {
        // cleanup any pending stuff, but no exceptions allowed
    }
};

// TODO: make this test faster

TEST_F(PingClientTest, Test) {
    Client c(_port);

    for (auto i = 0; i < 3; ++i) {
        auto msg = c.recv();
        auto obj = nlohmann::json::parse(msg);
        std::string question = obj["question"];
        ASSERT_NE(std::string::npos, question.find("Are you alive?"));
        nlohmann::json answer = {
            {"answer", "Yes"}
        };
        c.send(answer.dump());
    }
    boost::this_thread::sleep(boost::posix_time::seconds(20));

    auto fun = [](Client & c) {
        for (auto i = 0; i < 10; ++i) {
            c.send(std::to_string(i) + " message");
        }
    };
    ASSERT_THROW(fun(c), ReggataException);
}