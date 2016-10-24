#pragma once
#include "cmd_repo.h"

struct CmdAddTags: public CmdRepo {
	static const std::string NAME;

	std::string _file;
	std::vector<std::string> _tags;

	CmdAddTags(const std::string& id, Cmd::SendResult sendResult);
	virtual ~CmdAddTags() = default;

	static std::unique_ptr<CmdAddTags> fromJson(const json::json& j, Cmd::SendResult sendResult); // TODO: use unique_ptr for result

	virtual std::string path() const;
	virtual void execute();
};
