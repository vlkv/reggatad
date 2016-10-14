#include "cmd_add_tags.h"

CmdAddTags::CmdAddTags() {
	// TODO Auto-generated constructor stub
}

std::unique_ptr<CmdAddTags> CmdAddTags::parse(const boost::property_tree::ptree& pt) {
	// TODO
	return std::unique_ptr<CmdAddTags>(new CmdAddTags());
}

