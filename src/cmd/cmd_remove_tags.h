#pragma once
#include "cmd_repo.h"
#include <json_map.h>

struct CmdRemoveTags : public CmdRepo {
    static const std::string NAME;
    static const JsonMap::ParseMap<CmdRemoveTags> parseMap;

    std::string _file;
    std::vector<std::string> _tags;

    CmdRemoveTags(const std::string& id, Cmd::SendResult sendResult);
    virtual ~CmdRemoveTags() = default;

    virtual json::json execute();

    virtual std::string path() const;
};