#pragma once
#include <nlohmann/json.hpp>

struct RepoInfo {
    std::string _rootDir;
    std::string _dbDir;
    
    nlohmann::json toJson() {
        // TODO: use automatic serialization here
        nlohmann::json res;
        res["root_dir"] = _rootDir;
        res["db_dir"] = _dbDir;
        return res;
    }
    
    void fromJson(const nlohmann::json& obj) {
        this->_rootDir = obj.at("root_dir");
        this->_dbDir = obj.at("db_dir");
    }
};
