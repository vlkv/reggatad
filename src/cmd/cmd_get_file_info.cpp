#include "cmd_get_file_info.h"
#include <reggata_exceptions.h>
#include <status_code.h>
#include <repo.h>
#include <boost/format.hpp>
#include <boost/assign.hpp>

CmdGetFileInfo::CmdGetFileInfo(const std::string& id, Cmd::SendResult sendResult) :
CmdRepo(id, sendResult) {
}

const std::string CmdGetFileInfo::_name = "get_file_info";

const JsonMap::ParseMap<CmdGetFileInfo> CmdGetFileInfo::_parseMap = boost::assign::list_of
        (JsonMap::mapValue("file", &CmdGetFileInfo::_file));

boost::filesystem::path CmdGetFileInfo::path() const {
    return _file;
}

nlohmann::json CmdGetFileInfo::execute() {
    boost::filesystem::path filePath(_file);
    if (!filePath.is_absolute()) {
        throw StatusCodeException(StatusCode::CLIENT_ERROR,
                boost::format("file=%1% must be an absolute path") % _file);
    }
    auto fi = _repo->getFileInfo(filePath);
    auto fiJson = fi.toJson();
    nlohmann::json res = {
        {"code", StatusCode::OK},
        {"data", fiJson}
    };
    return res;
}