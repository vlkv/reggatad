#include "cmd_remove_tags.h"
#include <boost/assign.hpp>

CmdRemoveTags::CmdRemoveTags(const std::string& id, Cmd::SendResult sendResult) :
	CmdRepo(id, sendResult) {
}

const std::string CmdRemoveTags::NAME = "remove_tags";

const JsonMap::ParseMap<CmdRemoveTags> CmdRemoveTags::parseMap = boost::assign::list_of
    (JsonMap::mapValue("file", &CmdRemoveTags::_file))
    (JsonMap::mapArray("tags", &CmdRemoveTags::_tags));

std::string CmdRemoveTags::path() const {
	return _file;
}

json::json CmdRemoveTags::execute() {
    // TODO
	return json::json{};
}
