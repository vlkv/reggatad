#pragma once
#include "cmd_proc.h"
#include <json_map.h>

struct CmdOpenRepo : public CmdProc {
    static const std::string _name;
    static const JsonMap::ParseMap<CmdOpenRepo> _parseMap;

    std::string _rootDir;
    std::string _dbDir;
    bool _initIfNotExists = false;

    CmdOpenRepo(const std::string& id, Cmd::SendResult sendResult);
    virtual ~CmdOpenRepo() = default;

    virtual nlohmann::json execute();
};
