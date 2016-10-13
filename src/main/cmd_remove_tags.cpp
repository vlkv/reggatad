#include "cmd_remove_tags.h"

CmdRemoveTags::CmdRemoveTags() {
	// TODO Auto-generated constructor stub
}

std::unique_ptr<CmdRemoveTags> CmdRemoveTags::parse(const boost::property_tree::ptree& pt) {
	// TODO
	return std::unique_ptr<CmdRemoveTags>(new CmdRemoveTags());
}
