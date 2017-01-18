#pragma once
#include <repo.h>
#include <boost/filesystem/path.hpp>
#include <string>
#include <memory>
#include <set>

class Node {
public:
    virtual ~Node() = default;
    
    virtual std::string str() = 0;
    
    virtual std::set<std::string> findFileIdsIn(std::shared_ptr<Repo> repo, 
        const boost::filesystem::path& dirRelPath) = 0;
};
