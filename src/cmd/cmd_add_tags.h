#pragma once
#include "cmd_repo.h"
#include <json_map.h>

struct CmdAddTags: public CmdRepo {
	static const std::string NAME;
        static const JsonMap::ParseMap<CmdAddTags> parseMap;

	std::string _file;
	std::vector<std::string> _tags;

	CmdAddTags(const std::string& id, Cmd::SendResult sendResult);
	virtual ~CmdAddTags() = default;

	virtual std::string path() const;
	virtual json::json execute();
};
