#include "cmd_remove_tags.h"

CmdRemoveTags::CmdRemoveTags(const std::string& id) :
	Cmd(id) {
}

const std::string CmdRemoveTags::NAME = "remove_tags";

std::unique_ptr<CmdRemoveTags> CmdRemoveTags::parse(const json::json& j) {
	std::string cmd = j["cmd"];
	BOOST_ASSERT_MSG(cmd == CmdRemoveTags::NAME, "Bad cmd");

	std::string id = j["id"];
	auto res = std::unique_ptr<CmdRemoveTags>(new CmdRemoveTags(id));

	auto args = j["args"];
	res->_file = args["file"];
	for (auto& tag : args["tags"]) {
		res->_tags.push_back(tag);
	}
	return res;
}
