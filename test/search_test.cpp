#include "client.h"
#include "common.h"
#include "file_info.h"
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
        _rgen.seed(42); // NOTE: seed with constant gives us the same random sequence at every tests run
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

    FileInfo findAndCreate(const std::string& path, const nlohmann::json& fileInfos) {
        auto fileInfoIt = std::find_if(fileInfos.begin(), fileInfos.end(),
                [path](const nlohmann::json & elem) -> bool {
                    return elem.at("path") == path;
                });
        FileInfo result;
        result.fromJson(*fileInfoIt);
        return result;
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

TEST_F(SearchTest, Tag1AndTag2) {
    Client c(_port);

    nlohmann::json cmd = {
        {"id", "2"},
        {"cmd", "search"},
        {"args",
            {
                {"dir", (_workDir).c_str()},
                {"query", "River & Boat"}
            }}
    };
    c.send(cmd.dump());
    auto msg = c.recv();
    auto obj = nlohmann::json::parse(msg);
    ASSERT_EQ("2", obj["id"]);
    ASSERT_EQ(StatusCode::OK, obj["code"]);
    auto dataObj = obj["data"];
    ASSERT_EQ(4, dataObj.size());
    {
        FileInfo fi = findAndCreate("foo/baz/three/3", dataObj);
        std::set<std::string> tagsExpected{"Boat", "Nissan Marine", "River"};
        ASSERT_EQ(tagsExpected, fi._tags);
    }
    {
        FileInfo fi = findAndCreate(".gitignore", dataObj);
        std::set<std::string> tagsExpected{"Boat", "Nissan Marine", "River"};
        ASSERT_EQ(tagsExpected, fi._tags);
    }
    {
        FileInfo fi = findAndCreate("foo/1", dataObj);
        std::set<std::string> tagsExpected{"Boat", "Cool", "Fishing", "Jaw", "River", "Water"};
        ASSERT_EQ(tagsExpected, fi._tags);
    }
    {
        FileInfo fi = findAndCreate("foo/bar/1", dataObj);
        std::set<std::string> tagsExpected{"Boat", "Cool", "Line", "Mercury", "Nissan Marine", "Pike", "River"};
        ASSERT_EQ(tagsExpected, fi._tags);
    }
}

TEST_F(SearchTest, Tag1AndTag2InSubDir) {
    Client c(_port);

    auto obj = c.search(_workDir / "foo", "River Boat");
    ASSERT_EQ(StatusCode::OK, obj["code"]);
    auto dataObj = obj["data"];
    ASSERT_EQ(3, dataObj.size());
    {
        FileInfo fi = findAndCreate("foo/baz/three/3", dataObj);
        std::set<std::string> tagsExpected{"Boat", "Nissan Marine", "River"};
        ASSERT_EQ(tagsExpected, fi._tags);
    }
    {
        FileInfo fi = findAndCreate("foo/1", dataObj);
        std::set<std::string> tagsExpected{"Boat", "Cool", "Fishing", "Jaw", "River", "Water"};
        ASSERT_EQ(tagsExpected, fi._tags);
    }
    {
        FileInfo fi = findAndCreate("foo/bar/1", dataObj);
        std::set<std::string> tagsExpected{"Boat", "Cool", "Line", "Mercury", "Nissan Marine", "Pike", "River"};
        ASSERT_EQ(tagsExpected, fi._tags);
    }
}

TEST_F(SearchTest, NotNonExistingTag) {
    Client c(_port);

    auto obj = c.search(_workDir, "!NonExistingTag"); // NOTE: it queries all the files in database
    ASSERT_EQ(StatusCode::OK, obj["code"]);
    auto dataObj = obj["data"];
    ASSERT_EQ(14, dataObj.size());
    {
        FileInfo fi = findAndCreate("foo/baz/one/1", dataObj);
        std::set<std::string> tagsExpected{"Good", "Line", "Pike", "River", "Sea"};
        ASSERT_EQ(tagsExpected, fi._tags);
    }
    {
        FileInfo fi = findAndCreate("foo/bar/3", dataObj);
        std::set<std::string> tagsExpected{"Pike", "Water", "Yamaha"};
        ASSERT_EQ(tagsExpected, fi._tags);
    }
    {
        FileInfo fi = findAndCreate("foo/baz/three/2", dataObj);
        std::set<std::string> tagsExpected{"Anchor", "Hook", "Mercury", "Nissan Marine", "River"};
        ASSERT_EQ(tagsExpected, fi._tags);
    }
    {
        FileInfo fi = findAndCreate("foo/2", dataObj);
        std::set<std::string> tagsExpected{"Boat", "Cool", "Don", "Fishing", "Mercury", "Sea"};
        ASSERT_EQ(tagsExpected, fi._tags);
    }
    {
        FileInfo fi = findAndCreate("foo/bar/2", dataObj);
        std::set<std::string> tagsExpected{"Boat", "Don", "Motor"};
        ASSERT_EQ(tagsExpected, fi._tags);
    }
    {
        FileInfo fi = findAndCreate("foo/baz/three/3", dataObj);
        std::set<std::string> tagsExpected{"Boat", "Nissan Marine", "River"};
        ASSERT_EQ(tagsExpected, fi._tags);
    }
    {
        FileInfo fi = findAndCreate("foo/bar/1", dataObj);
        std::set<std::string> tagsExpected{"Boat", "Cool", "Line", "Mercury", "Nissan Marine", "Pike", "River"};
        ASSERT_EQ(tagsExpected, fi._tags);
    }
    {
        FileInfo fi = findAndCreate("foo/baz/two/2", dataObj);
        std::set<std::string> tagsExpected{"Cool", "Deep", "Good", "Line"};
        ASSERT_EQ(tagsExpected, fi._tags);
    }
    {
        FileInfo fi = findAndCreate(".gitignore", dataObj);
        std::set<std::string> tagsExpected{"Boat", "Nissan Marine", "River"};
        ASSERT_EQ(tagsExpected, fi._tags);
    }
    {
        FileInfo fi = findAndCreate("foo/1", dataObj);
        std::set<std::string> tagsExpected{"Boat", "Cool", "Fishing", "Jaw", "River", "Water"};
        ASSERT_EQ(tagsExpected, fi._tags);
    }
    {
        FileInfo fi = findAndCreate("foo/baz/three/1", dataObj);
        std::set<std::string> tagsExpected{"Motor", "Water"};
        ASSERT_EQ(tagsExpected, fi._tags);
    }
    {
        FileInfo fi = findAndCreate("foo/baz/three/4", dataObj);
        std::set<std::string> tagsExpected{"Nissan Marine"};
        ASSERT_EQ(tagsExpected, fi._tags);
    }
    {
        FileInfo fi = findAndCreate("foo/baz/two/1", dataObj);
        std::set<std::string> tagsExpected{"Nissan Marine", "Sea", "Yamaha"};
        ASSERT_EQ(tagsExpected, fi._tags);
    }
    {
        FileInfo fi = findAndCreate("file", dataObj);
        std::set<std::string> tagsExpected{"Line"};
        ASSERT_EQ(tagsExpected, fi._tags);
    }

}

TEST_F(SearchTest, ComplexQuery) {
    Client c(_port);

    auto obj = c.search(_workDir / "foo", "Yamaha | Mercury !Jaw Sea");
    ASSERT_EQ(StatusCode::OK, obj["code"]);
    auto dataObj = obj["data"];
    ASSERT_EQ(2, dataObj.size());
    {
        FileInfo fi = findAndCreate("foo/2", dataObj);
        std::set<std::string> tagsExpected{"Boat", "Cool", "Don", "Fishing", "Mercury", "Sea"};
        ASSERT_EQ(tagsExpected, fi._tags);
    }
    {
        FileInfo fi = findAndCreate("foo/baz/two/1", dataObj);
        std::set<std::string> tagsExpected{"Nissan Marine", "Sea", "Yamaha"};
        ASSERT_EQ(tagsExpected, fi._tags);
    }
}
