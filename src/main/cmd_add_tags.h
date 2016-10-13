#pragma once
#include "cmd.h"

class CmdAddTags: public Cmd {
public:
	std::string _file;
	std::vector<std::string> _tags;

	CmdAddTags();
	virtual ~CmdAddTags() = default;

	static std::unique_ptr<CmdAddTags> parse(const boost::property_tree::ptree& pt);
};
