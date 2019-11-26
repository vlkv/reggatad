#include "testing.h"
#include <boost/filesystem/path.hpp>
#include <random>

namespace {

std::vector<std::string> allTags{"Sea", "Boat", "Fishing", "Mercury", "Pike",
    "Motor", "Line", "Hook", "Water", "River", "Deep", "Good", "Anchor", "Cool",
    "Don", "Jaw", "Yamaha", "Nissan Marine"};
std::default_random_engine rgen{42}; // NOTE: seed with constant gives us the same random sequence at every tests run
std::uniform_int_distribution<int> idist;

std::vector<std::string> getNRandomTags(size_t n) {
    std::vector<std::string> result;
    for (size_t i = 0; i < n; ++i) {
        auto num = idist(rgen);
        auto index = num % allTags.size();
        result.push_back(allTags.at(index));
    }
    return result;
}

} // namespace

namespace reggatad::testing {

void initTestingRepo(const boost::filesystem::path& repoRootDir) {
    const auto& dbDir(repoRootDir / ".reggata");
    
    boost::filesystem::remove_all(dbDir);
    Repo repo(repoRootDir, dbDir, true);
    
    std::vector<std::string> paths;
    for (auto&& entry : boost::filesystem::recursive_directory_iterator(repoRootDir)) {
        auto p = std::mismatch(dbDir.begin(), dbDir.end(), entry.path().begin());
        if (p.first == dbDir.end()) {
            continue;
        }
        if (entry.status().type() != boost::filesystem::file_type::regular_file) {
            continue;
        }
        paths.push_back(entry.path().string());
    }
    std::sort(paths.begin(), paths.end()); // Need a deterministic order of files traversal
    for (const auto& path : paths) {
        auto n = idist(rgen) % 10;
        auto tags = getNRandomTags(n);
        repo.addTags(path, tags);
    }
    repo.stop();
}

}