#include "cmd_get_file_info.h"
#include "status_code.h"
#include <repo.h>
#include <boost/assign.hpp>

CmdGetFileInfo::CmdGetFileInfo(const std::string& id, Cmd::SendResult sendResult) :
CmdRepo(id, sendResult) {
}

const std::string CmdGetFileInfo::NAME = "get_file_info";

const JsonMap::ParseMap<CmdGetFileInfo> CmdGetFileInfo::parseMap = boost::assign::list_of
        (JsonMap::mapValue("file", &CmdGetFileInfo::_file));

std::string CmdGetFileInfo::path() const {
    return _file;
}

json::json CmdGetFileInfo::execute() {
    auto fi = _repo->getFileInfo(_file);
    auto res = fi.toJson();
    res["code"] = StatusCode::OK;
    return res;
}