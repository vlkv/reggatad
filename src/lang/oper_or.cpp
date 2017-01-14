#include "oper_or.h"
#include <boost/format.hpp>
#include <algorithm>

OperOr::OperOr(std::shared_ptr<Node> left, std::shared_ptr<Node> right) :
_left(left), _right(right) {
}

std::string OperOr::str() {
    return (boost::format("(OR %1% %2%)") % _left->str() % _right->str()).str();
}

std::unordered_set<std::string> OperOr::findFileIdsIn(std::shared_ptr<Repo> repo,
        const boost::filesystem::path& dirRelPath) {
    auto idsL = _left->findFileIdsIn(repo, dirRelPath);
    auto idsR = _right->findFileIdsIn(repo, dirRelPath);
    std::unordered_set<std::string> result;
    std::set_union(idsL.begin(), idsL.end(), idsR.begin(), idsR.end(), std::inserter(result, result.begin()));
    return result;
}
