#include "tag.h"

Tag::Tag(const std::string& name) : _name(name) {
}

std::string Tag::str() {
    return _name;
}

std::set<std::string> Tag::findFileIdsIn(std::shared_ptr<Repo> repo,
        const boost::filesystem::path& dirRelPath) {
    auto result = repo->findFileIds(_name, dirRelPath);
    return result;
}
