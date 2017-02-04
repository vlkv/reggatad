#include "cmd_remove_tags.h"
#include <reggata_exceptions.h>
#include <status_code.h>
#include <repo.h>
#include <boost/format.hpp>
#include <boost/assign.hpp>

CmdRemoveTags::CmdRemoveTags(const std::string& id, Cmd::SendResult sendResult) :
CmdRepo(id, sendResult) {
}

const std::string CmdRemoveTags::_name = "remove_tags";

const JsonMap::ParseMap<CmdRemoveTags> CmdRemoveTags::_parseMap = boost::assign::list_of
        (JsonMap::mapValue("file", &CmdRemoveTags::_file))
(JsonMap::mapArray("tags", &CmdRemoveTags::_tags));

boost::filesystem::path CmdRemoveTags::path() const {
    return _file;
}

nlohmann::json CmdRemoveTags::execute() {
    boost::filesystem::path filePath(_file);
    if (!filePath.is_absolute()) {
        throw StatusCodeException(StatusCode::CLIENT_ERROR,
                boost::format("file=%1% must be an absolute path") % _file);
    }
    _repo->removeTags(filePath, _tags);
    return nlohmann::json{
        {"code", StatusCode::OK}};
}
