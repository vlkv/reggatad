#include "cmd.h"
#include "cmd_add_tags.h"
#include "cmd_remove_tags.h"
#include "reggata_exceptions.h"

#include <boost/log/trivial.hpp>

Cmd::Cmd(const std::string& id) : _id(id) {
}

std::unique_ptr<Cmd> Cmd::parse(const json::json& j) {
	BOOST_LOG_TRIVIAL(debug) << "Cmd::parse";
	std::string cmdStr = j["cmd"];
	if (cmdStr == CmdAddTags::NAME) {
		return CmdAddTags::parse(j);
	} else if (cmdStr == CmdRemoveTags::NAME) {
		return CmdRemoveTags::parse(j);
	} else {
		throw new ReggataException(std::string("Unexpected command: ") + cmdStr);
	}
}
