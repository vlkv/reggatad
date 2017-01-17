#include "client.h"
#include "common.h"
#include <status_code.h>
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
#include <random>

class SearchTest : public testing::Test {
public:
    const int _port = 9100;
    std::unique_ptr<Application> _app;
    boost::thread _t;
    boost::filesystem::path _workDir;

    std::vector<std::string> _allTags{"Sea", "Boat", "Fishing", "Mercury", "Pike",
        "Motor", "Line", "Hook", "Water", "River", "Deep", "Good", "Anchor", "Cool",
        "Don", "Jaw", "Yamaha", "Nissan Marine"};
    std::default_random_engine _rgen;
    std::uniform_int_distribution<int> _idist;

    SearchTest() :
    _app(new Application(_port, 0)),
    _t(&SearchTest::startReggataApp, this),
    _workDir(boost::filesystem::canonical("./test_data/repo")),
    _idist(0, _allTags.size() - 1) {
        _rgen.seed(42);
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

        for (auto&& entry : boost::filesystem::recursive_directory_iterator(_workDir)) {
            //std::cout << "FILE: " << entry.path() << std::endl;
            auto p = std::mismatch(dbDir.begin(), dbDir.end(), entry.path().begin());
            if (p.first == dbDir.end()) {
                //std::cout << "SKIPPING" << std::endl;
                continue;
            }
            if (entry.status().type() != boost::filesystem::file_type::regular_file) {
                continue;
            }
            auto n = _idist(_rgen) % 10;
            auto tags = getNRandomTags(n);
            auto r2 = c.addTags(entry.path(), tags);
            ASSERT_EQ(StatusCode::OK, r2["code"]);
        }
    }

    std::vector<std::string> getNRandomTags(size_t n) {
        std::vector<std::string> result;
        for (size_t i = 0; i < n; ++i) {
            auto num = _idist(_rgen);
            auto index = num % _allTags.size();
            result.push_back(_allTags.at(index));
        }
        return result;
    }

    void TearDown() {
        _app->stop();
        _t.join();
    }

    ~SearchTest() {
        // cleanup any pending stuff, but no exceptions allowed
    }
};

TEST_F(SearchTest, Simple) {
    Client c(_port);
    {
        json::json cmd = {
            {"id", "2"},
            {"cmd", "search"},
            {"args",
                {
                    {"dir", (_workDir).c_str()},
                    {"query", "River Boat"}
                }}
        };
        c.send(cmd.dump());
        auto msg = c.recv();
        auto obj = nlohmann::json::parse(msg);
        ASSERT_EQ("2", obj["id"]);
        ASSERT_EQ(StatusCode::OK, obj["code"]);
        auto dataObj = obj["data"];
        ASSERT_EQ(2, dataObj.size());
    }

    {// TODO: check that search results are true with get file info?..

    }
}
