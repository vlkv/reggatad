#pragma once
#include <nlohmann/json.hpp>

struct FileInfo {
    std::string _path;
    std::vector<std::string> _tags;
    size_t _size;
    
    nlohmann::json toJson() {
        // TODO: use automatic serialization here
        nlohmann::json res;
        res["path"] = _path;
        res["tags"] = _tags;
        res["size"] = _size;
        return res;
    }
};
