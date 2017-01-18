#include "oper_not.h"
#include <boost/format.hpp>

OperNot::OperNot(std::shared_ptr<Node> node) : _node(node) {
}

std::string OperNot::str() {
    return (boost::format("(NOT %1%)") % _node->str()).str();
}

// TODO: Most of the time users would search for "TagX AND NOT TagY", "AND NOT" combination could be optimized

std::set<std::string> OperNot::findFileIdsIn(std::shared_ptr<Repo> repo,
        const boost::filesystem::path& dirRelPath) {
    auto ids = _node->findFileIdsIn(repo, dirRelPath);
    auto result = repo->findAllFileIdsExcept(ids, dirRelPath);
    return result;
}
