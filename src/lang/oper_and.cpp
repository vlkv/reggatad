#include "oper_and.h"
#include <boost/format.hpp>

OperAnd::OperAnd(std::shared_ptr<Node> left, std::shared_ptr<Node> right) :
_left(left), _right(right) {
}

std::string OperAnd::str() {
    return (boost::format("(AND %1% %2%)") % _left->str() % _right->str()).str();
}
