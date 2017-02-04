#include "cmd_close_repo.h"
#include <reggata_exceptions.h>
#include <status_code.h>
#include <processor.h>
#include <json_map.h>
#include <boost/format.hpp>
#include <boost/assign.hpp>
#include <boost/filesystem.hpp>

CmdCloseRepo::CmdCloseRepo(const std::string& id, Cmd::SendResult sendResult) :
CmdProc(id, sendResult) {
}

const std::string CmdCloseRepo::_name = "close_repo";

const JsonMap::ParseMap<CmdCloseRepo> CmdCloseRepo::_parseMap = boost::assign::list_of
        (JsonMap::mapValue("root_dir", &CmdCloseRepo::_rootDir));

nlohmann::json CmdCloseRepo::execute() {
    boost::filesystem::path rootDirPath(_rootDir);
    if (!rootDirPath.is_absolute()) {
        throw StatusCodeException(StatusCode::CLIENT_ERROR,
                boost::format("root_dir=%1% must be an absolute path") % _rootDir);
    }

    _proc->closeRepo(rootDirPath);
    nlohmann::json res = {
        {"code", StatusCode::OK}
    };
    return res;
}
