#pragma once
#include <cmd_proc.h>
#include <json_map.h>

struct CmdGetReposInfo  : public CmdProc {
    static const std::string _name;
    static const JsonMap::ParseMap<CmdGetReposInfo> _parseMap;

    CmdGetReposInfo(const std::string& id, Cmd::SendResult sendResult);
    virtual ~CmdGetReposInfo() = default;

    virtual nlohmann::json execute();
};
