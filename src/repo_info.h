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
    
    static RepoInfo fromJson(const nlohmann::json& obj) {
        RepoInfo ri;
        ri._rootDir = obj.at("root_dir");
        ri._dbDir = obj.at("db_dir");
        return ri;
    }
};
