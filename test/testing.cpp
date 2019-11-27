#include "testing.h"
#include <boost/filesystem/path.hpp>
#include <boost/format.hpp>
#include <reggata_exceptions.h>

namespace {

} // namespace

namespace reggatad::testing {

void initTestingRepo(const boost::filesystem::path& repoRootDir, const boost::filesystem::path& repoMetaFile) {
    const auto& dbDir(repoRootDir / ".reggata");
    
    std::ifstream t(repoMetaFile.string());
    std::stringstream buffer;
    buffer << t.rdbuf();
    const auto repoMetaJson = buffer.str();
    const auto repoMeta = nlohmann::json::parse(repoMetaJson);

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

    for (const auto& path : paths) {
        auto relPath = repo.makeRelativePath(path);
        const auto elem = repoMeta.find(relPath.string());
        if (elem != repoMeta.cend()) {
            repo.addTags(path, elem.value()["tags"]);
        } else {
            throw ReggataException((boost::format("Could not find meta info about path: %1%") % path).str());
        }
    }
    repo.stop();
}

}