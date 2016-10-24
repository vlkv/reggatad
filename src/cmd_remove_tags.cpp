#include "cmd_remove_tags.h"

CmdRemoveTags::CmdRemoveTags(const std::string& id, Cmd::SendResult sendResult) :
	CmdRepo(id, sendResult) {
}

const std::string CmdRemoveTags::NAME = "remove_tags";

std::unique_ptr<CmdRemoveTags> CmdRemoveTags::fromJson(const json::json& j, Cmd::SendResult sendResult) {
	std::string cmd = j["cmd"];
	BOOST_ASSERT_MSG(cmd == CmdRemoveTags::NAME, "Bad cmd");

	std::string id = j["id"];
	auto res = std::unique_ptr<CmdRemoveTags>(new CmdRemoveTags(id, sendResult));

	auto args = j["args"];
	res->_file = args["file"];
	for (auto& tag : args["tags"]) {
		res->_tags.push_back(tag);
	}
	return res;
}

std::string CmdRemoveTags::path() const {
	return _file;
}

void CmdRemoveTags::execute() {
	// TODO
}
