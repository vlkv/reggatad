#include "cmd_add_tags.h"

CmdAddTags::CmdAddTags(const std::string& id) :
	Cmd(id) {
}

const std::string CmdAddTags::NAME = "add_tags";

std::unique_ptr<CmdAddTags> CmdAddTags::parse(const json::json& j) {
	std::string cmd = j["cmd"];
	BOOST_ASSERT_MSG(cmd == CmdAddTags::NAME, "Bad cmd");

	const std::string id = j["id"];
	auto res = std::unique_ptr<CmdAddTags>(new CmdAddTags(id));

	auto args = j["args"];
	res->_file = args["file"];
	for (auto& tag : args["tags"]) {
		res->_tags.push_back(tag);
	}
	return res;
}

