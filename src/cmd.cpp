#include "cmd.h"
#include "cmd_add_tags.h"
#include "cmd_remove_tags.h"
#include "cmd_open_repo.h"
#include "reggata_exceptions.h"

#include <boost/log/trivial.hpp>

Cmd::Cmd(const std::string& id, Cmd::SendResult sendResult) :
	_id(id),
	_sendResult(sendResult) {
}

std::unique_ptr<Cmd> Cmd::fromJson(const json::json& j, Cmd::SendResult sendResult) {
	BOOST_LOG_TRIVIAL(debug) << "Cmd::fromJson";
	std::string cmdStr = j["cmd"];
	if (cmdStr == CmdAddTags::NAME) {
		return CmdAddTags::fromJson(j, sendResult);
	} else if (cmdStr == CmdRemoveTags::NAME) {
		return CmdRemoveTags::fromJson(j, sendResult);
	} else if (cmdStr == CmdOpenRepo::NAME) {
		return CmdOpenRepo::fromJson(j, sendResult);
	} else {
		throw new ReggataException(std::string("Unexpected command: ") + cmdStr);
	}
}
