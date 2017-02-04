#pragma once
#include "cmd_repo.h"
#include <json_map.h>

struct CmdRemoveTags : public CmdRepo {
    static const std::string _name;
    static const JsonMap::ParseMap<CmdRemoveTags> _parseMap;

    std::string _file;
    std::vector<std::string> _tags;

    CmdRemoveTags(const std::string& id, Cmd::SendResult sendResult);
    virtual ~CmdRemoveTags() = default;

    virtual nlohmann::json execute();

    virtual boost::filesystem::path path() const;
};
