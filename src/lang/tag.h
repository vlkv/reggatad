#pragma once
#include "node.h"
#include <string>

class Tag : public Node {
    std::string _name;
    
public:
    Tag(const std::string& name);
    virtual ~Tag() = default;
    
    virtual std::string str();
    virtual std::unordered_set<std::string> findFileIdsIn(std::shared_ptr<Repo> repo, 
        const boost::filesystem::path& dirRelPath);
};
