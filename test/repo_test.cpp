#include "client.h"
#include "common.h"
#include "file_info.h"
#include <status_code.h>
#include <repo.h>
#include <testing.h>
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

using namespace reggatad::testing;

class RepoTest : public testing::Test {
public:
    std::unique_ptr<Repo> _repo;
    boost::filesystem::path _workDir;

    RepoTest()
        : _workDir(boost::filesystem::canonical("./test_data/repo"))
    {
    }

    void SetUp() {
        initTestingRepo(_workDir);
        _repo.reset(new Repo(_workDir, _workDir / ".reggata"));
    }

    void TearDown() {
        _repo->stop();
        _repo.release();
    }

    ~RepoTest() {
        // cleanup any pending stuff, but no exceptions allowed
    }
};

TEST_F(RepoTest, CreateOneNewFile) {
    std::fstream file((_workDir / "test.txt").c_str(), std::fstream::in | std::fstream::out | std::fstream::trunc);
    file << "hello, world!" << std::endl;
    file.close();

    // Wait for DirWatched to catch the filesystem events...
    boost::this_thread::sleep(boost::posix_time::millisec(1000));
 
    // TODO: Assert on repo actions...
}

