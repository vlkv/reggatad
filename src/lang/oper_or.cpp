#include "oper_or.h"
#include <boost/format.hpp>

OperOr::OperOr(std::shared_ptr<Node> left, std::shared_ptr<Node> right) :
_left(left), _right(right) {
}

std::string OperOr::str() {
    return (boost::format("(OR %1% %2%)") % _left->str() % _right->str()).str();
}
