#pragma once
#include "cmd_repo.h"
#include <json_map.h>

struct CmdSearch : public CmdRepo {
    static const std::string _name;
    static const JsonMap::ParseMap<CmdSearch> _parseMap;

    std::string _dir;
    std::string _query;
    
    CmdSearch(const std::string& id, Cmd::SendResult sendResult);
    virtual ~CmdSearch() = default;
    
    virtual boost::filesystem::path path() const;
    virtual json::json execute();
};
