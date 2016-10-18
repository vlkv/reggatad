#include "cmd_add_tags.h"

CmdAddTags::CmdAddTags(const std::string& id, Cmd::SendResult sendResult) :
	CmdRepo(id, sendResult) {
}

const std::string CmdAddTags::NAME = "add_tags";

CmdAddTags* CmdAddTags::fromJson(const json::json& j, Cmd::SendResult sendResult) {
	std::string cmd = j["cmd"];
	BOOST_ASSERT_MSG(cmd == CmdAddTags::NAME, "Bad cmd");

	const std::string id = j["id"];
	auto res = std::unique_ptr<CmdAddTags>(new CmdAddTags(id, sendResult));

	auto args = j["args"];
	res->_file = args["file"];
	for (auto& tag : args["tags"]) {
		res->_tags.push_back(tag);
	}
	return res.release();
}

std::string CmdAddTags::path() const {
	return _file;
}

void CmdAddTags::execute() {

}
