#include "tag.h"

Tag::Tag(const std::string& name) : _name(name) {
}

std::string Tag::str() {
    return _name;
}

std::unordered_set<std::string> Tag::findFileIdsIn(std::shared_ptr<Repo> repo,
        const boost::filesystem::path& dirRelPath) {
    // TODO: search in repo for files with tag _name and in dirRelPath
    return std::unordered_set<std::string>();
}
