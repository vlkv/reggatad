#pragma once
#include "cmd.h"

class CmdRemoveTags: public Cmd {
public:
	std::string _file;
	std::vector<std::string> _tags;

	CmdRemoveTags();
	virtual ~CmdRemoveTags() = default;

	static std::unique_ptr<CmdRemoveTags> parse(const boost::property_tree::ptree& pt);
};
