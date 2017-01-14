#pragma once
#include "node.h"
#include <memory>

class OperNot : public Node {
    std::shared_ptr<Node> _node;
    
public:
    OperNot(std::shared_ptr<Node> node);
    virtual ~OperNot() = default;

    virtual std::string str();
};
