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
#include <testing.h>
#include <memory>
#include <iostream>
#include <sstream>

using namespace reggatad::testing;

class SearchTest : public testing::Test {
public:
    boost::filesystem::path _testDataDir;
    boost::filesystem::path _workDir;
    const int _port = 9100;
    std::unique_ptr<Application> _app;
    boost::thread _t;

    SearchTest()
        : _testDataDir(boost::filesystem::canonical("./test_data"))
        , _workDir(_testDataDir / "repo")
        , _app(new Application(_port, 0))
        , _t(&SearchTest::startReggataApp, this)
    {
    }

    void startReggataApp() {
        _app->start();
    }

    void SetUp() {
        Client c(_port);
        boost::filesystem::path dbDir(_workDir / ".reggata");
        initTestingRepo(_workDir, _testDataDir / "repo.meta.json");
        c.openRepo(_workDir, dbDir);
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
