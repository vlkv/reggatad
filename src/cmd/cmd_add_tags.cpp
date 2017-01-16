#include "cmd_add_tags.h"
#include <reggata_exceptions.h>
#include <status_code.h>
#include <repo.h>
#include <boost/format.hpp>
#include <boost/assign.hpp>

CmdAddTags::CmdAddTags(const std::string& id, Cmd::SendResult sendResult) :
CmdRepo(id, sendResult) {
}

const std::string CmdAddTags::_name = "add_tags";

const JsonMap::ParseMap<CmdAddTags> CmdAddTags::_parseMap = boost::assign::list_of
        (JsonMap::mapValue("file", &CmdAddTags::_file))
(JsonMap::mapArray("tags", &CmdAddTags::_tags));

boost::filesystem::path CmdAddTags::path() const {
    return _file;
}

json::json CmdAddTags::execute() {
    boost::filesystem::path filePath(_file);
    if (!filePath.is_absolute()) {
        throw StatusCodeException(StatusCode::CLIENT_ERROR,
                boost::format("file=%1% must be an absolute path") % _file);
    }
    _repo->addTags(filePath, _tags);
    return json::json{
        {"code", StatusCode::OK}};
}
