#include "client.h"
#include "common.h"
#include "file_info.h"
#include <status_code.h>
#include <repo.h>
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

class RepoTest : public testing::Test {
public:
    std::unique_ptr<Repo> _repo;
    boost::filesystem::path _workDir;
    boost::filesystem::path _dbDir;

    std::vector<std::string> _allTags{"Sea", "Boat", "Fishing", "Mercury", "Pike",
        "Motor", "Line", "Hook", "Water", "River", "Deep", "Good", "Anchor", "Cool",
        "Don", "Jaw", "Yamaha", "Nissan Marine"};
    std::default_random_engine _rgen;
    std::uniform_int_distribution<int> _idist;

    RepoTest() :
    _workDir(boost::filesystem::canonical("./test_data/repo")),
    _dbDir(_workDir / ".reggata"),
    _idist(0, _allTags.size() - 1) {
        _rgen.seed(42); // NOTE: seed with constant gives us the same random sequence at every tests run
    }

    void SetUp() {
        // TODO: move to a testing helper utils code that initializes a new repo with files and tags
        boost::filesystem::remove_all(_dbDir);
        _repo.reset(new Repo(_workDir, _dbDir, true));
        
        for (auto&& entry : boost::filesystem::recursive_directory_iterator(_workDir)) {
            auto p = std::mismatch(_dbDir.begin(), _dbDir.end(), entry.path().begin());
            if (p.first == _dbDir.end()) {
                continue;
            }
            if (entry.status().type() != boost::filesystem::file_type::regular_file) {
                continue;
            }
            auto n = _idist(_rgen) % 10;
            auto tags = getNRandomTags(n);
            _repo->addTags(entry.path(), tags);
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
        _repo->stop();
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

