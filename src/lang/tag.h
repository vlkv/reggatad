#pragma once
#include "node.h"
#include <string>

class Tag : public Node {
    std::string _name;
    
public:
    Tag(const std::string& name);
    virtual ~Tag() = default;
private:

};
