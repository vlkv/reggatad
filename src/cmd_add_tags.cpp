#include "cmd_add_tags.h"
#include <boost/assign.hpp>

CmdAddTags::CmdAddTags(const std::string& id, Cmd::SendResult sendResult) :
	CmdRepo(id, sendResult) {
}

const std::string CmdAddTags::NAME = "add_tags";

const JsonMap::ParseMap<CmdAddTags> CmdAddTags::parseMap = boost::assign::list_of
    (JsonMap::mapValue("file", &CmdAddTags::_file))
    (JsonMap::mapArray("tags", &CmdAddTags::_tags));

std::string CmdAddTags::path() const {
	return _file;
}

json::json CmdAddTags::execute() {
    // TODO
	return json::json{};
}
