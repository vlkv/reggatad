#pragma once
#include "cmd_proc.h"
#include <json_map.h>

struct CmdCloseRepo : public CmdProc {
    static const std::string _name;
    static const JsonMap::ParseMap<CmdCloseRepo> _parseMap;

    std::string _rootDir;
    
    CmdCloseRepo(const std::string& id, Cmd::SendResult sendResult);
    virtual ~CmdCloseRepo() = default;

    virtual nlohmann::json execute();
};
