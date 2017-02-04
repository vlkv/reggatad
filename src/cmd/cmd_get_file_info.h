#pragma once
#include "cmd_repo.h"
#include <json_map.h>

struct CmdGetFileInfo : public CmdRepo {
    static const std::string _name;
    static const JsonMap::ParseMap<CmdGetFileInfo> _parseMap;

    std::string _file;
    
    CmdGetFileInfo(const std::string& id, Cmd::SendResult sendResult);
    virtual ~CmdGetFileInfo() = default;
    
    virtual boost::filesystem::path path() const;
    virtual nlohmann::json execute();
};
