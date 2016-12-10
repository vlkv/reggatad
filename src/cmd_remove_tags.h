#pragma once
#include "cmd_repo.h"

struct CmdRemoveTags: public CmdRepo {
	static const std::string NAME;

	std::string _file;
	std::vector<std::string> _tags;

	CmdRemoveTags(const std::string& id, Cmd::SendResult sendResult);
	virtual ~CmdRemoveTags() = default;

	static std::unique_ptr<CmdRemoveTags> fromJson(const json::json& j, Cmd::SendResult sendResult); // TODO: use unique_ptr for result

	virtual json::json execute();

	virtual std::string path() const;
};
