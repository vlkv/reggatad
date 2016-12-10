#include "cmd_open_repo.h"
#include "processor.h"

CmdOpenRepo::CmdOpenRepo(const std::string& id, Cmd::SendResult sendResult) :
	CmdProc(id, sendResult) {
}

const std::string CmdOpenRepo::NAME = "open_repo";

std::unique_ptr<CmdOpenRepo> CmdOpenRepo::fromJson(const json::json& j, SendResult sendResult) {
	BOOST_LOG_TRIVIAL(debug) << "CmdOpenRepo::fromJson";
	std::string cmd = j["cmd"];
	BOOST_ASSERT_MSG(cmd == CmdOpenRepo::NAME, "Bad cmd");

	const std::string id = j["id"];
	auto res = std::unique_ptr<CmdOpenRepo>(new CmdOpenRepo(id, sendResult));

	auto args = j["args"];
	res->_rootDir = args["root_dir"];
	res->_dbDir = args["db_dir"];
	res->_initIfNotExists = args["init_if_not_exists"];
	return res;
}

json::json CmdOpenRepo::execute() {
	_proc->openRepo(_rootDir, _dbDir);
	// TODO: use initIfNotExists flag

	json::json res = {
		{"ok", true}
	};
	return res;
}

