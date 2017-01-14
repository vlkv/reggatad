#pragma once
#include "node.h"
#include <memory>

class OperOr : public Node {
    std::shared_ptr<Node> _left;
    std::shared_ptr<Node> _right;
    
public:
    OperOr(std::shared_ptr<Node> left, std::shared_ptr<Node> right);
    virtual ~OperOr() = default;
    
    virtual std::string str();
    virtual std::unordered_set<std::string> findFileIdsIn(std::shared_ptr<Repo> repo, 
        const boost::filesystem::path& dirRelPath);
};
