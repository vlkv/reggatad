#include "oper_and.h"
#include <boost/format.hpp>
#include <algorithm>

OperAnd::OperAnd(std::shared_ptr<Node> left, std::shared_ptr<Node> right) :
_left(left), _right(right) {
}

std::string OperAnd::str() {
    return (boost::format("(AND %1% %2%)") % _left->str() % _right->str()).str();
}

std::set<std::string> OperAnd::findFileIdsIn(std::shared_ptr<Repo> repo,
        const boost::filesystem::path& dirRelPath) {
    auto idsL = _left->findFileIdsIn(repo, dirRelPath);
    auto idsR = _right->findFileIdsIn(repo, dirRelPath);
    std::set<std::string> result;
    std::set_intersection(idsL.begin(), idsL.end(), idsR.begin(), idsR.end(), std::inserter(result, result.begin()));
    return result;
}
