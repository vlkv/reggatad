#pragma once
#include "node.h"
#include <memory>

class OperAnd : public Node {
    std::shared_ptr<Node> _left;
    std::shared_ptr<Node> _right;
    
public:
    OperAnd(std::shared_ptr<Node> left, std::shared_ptr<Node> right);
    virtual ~OperAnd() = default;

    virtual std::string str();
    virtual std::set<std::string> findFileIdsIn(std::shared_ptr<Repo> repo, 
        const boost::filesystem::path& dirRelPath);
};
