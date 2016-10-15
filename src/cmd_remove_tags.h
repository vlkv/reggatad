#pragma once
#include "cmd.h"

class CmdRemoveTags: public Cmd {
public:
	static const std::string NAME;

	std::string _file;
	std::vector<std::string> _tags;

	CmdRemoveTags(const std::string& id);
	virtual ~CmdRemoveTags() = default;

	static std::unique_ptr<CmdRemoveTags> parse(const json::json& j);
};
