#include "oper_not.h"
#include <boost/format.hpp>

OperNot::OperNot(std::shared_ptr<Node> node) : _node(node) {
}

std::string OperNot::str() {
    return (boost::format("(NOT %1%)") % _node->str()).str();
}
