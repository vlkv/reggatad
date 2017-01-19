#pragma once
#include <nlohmann/json.hpp>
#include <set>

struct FileInfo {
    std::string _path;
    std::set<std::string> _tags;
    size_t _size;
    
    nlohmann::json toJson() {
        // TODO: use automatic serialization here
        nlohmann::json res;
        res["path"] = _path;
        res["tags"] = _tags;
        res["size"] = _size;
        return res;
    }
    
    void fromJson(const nlohmann::json& obj) {
        this->_path = obj.at("path");
        
        this->_tags.clear();
        auto array = obj.at("tags");
        for (auto it = array.begin(); it != array.end(); ++it) {
            this->_tags.insert(it->get<std::string>());
        }
        
        this->_size = obj.at("size");
    }
};
