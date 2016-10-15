#pragma once
#include "cmd.h"

class CmdAddTags: public Cmd {
public:

	static const std::string NAME;

	std::string _file;
	std::vector<std::string> _tags;

	CmdAddTags(const std::string& id);
	virtual ~CmdAddTags() = default;

	static std::unique_ptr<CmdAddTags> parse(const json::json& j);
};
