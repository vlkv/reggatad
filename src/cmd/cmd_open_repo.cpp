#include "cmd_open_repo.h"
#include <reggata_exceptions.h>
#include <status_code.h>
#include <processor.h>
#include <json_map.h>
#include <boost/format.hpp>
#include <boost/assign.hpp>
#include <boost/filesystem.hpp>

CmdOpenRepo::CmdOpenRepo(const std::string& id, Cmd::SendResult sendResult) :
CmdProc(id, sendResult) {
}

const std::string CmdOpenRepo::_name = "open_repo";

const JsonMap::ParseMap<CmdOpenRepo> CmdOpenRepo::_parseMap = boost::assign::list_of
        (JsonMap::mapValue("root_dir", &CmdOpenRepo::_rootDir))
(JsonMap::mapValue("db_dir", &CmdOpenRepo::_dbDir, std::string(".reggata")))
(JsonMap::mapValue("init_if_not_exists", &CmdOpenRepo::_initIfNotExists, false));

json::json CmdOpenRepo::execute() {
    boost::filesystem::path rootDirPath(_rootDir);
    if (!rootDirPath.is_absolute()) {
        throw StatusCodeException(StatusCode::CLIENT_ERROR,
                boost::format("root_dir=%1% must be an absolute path") % _rootDir);
    }

    boost::filesystem::path dbDirPath(_dbDir);
    if (dbDirPath.is_relative()) {
        dbDirPath = boost::filesystem::absolute(dbDirPath, _rootDir);
    }

    _proc->openRepo(rootDirPath, dbDirPath, _initIfNotExists);
    json::json res = {
        {"code", StatusCode::OK}
    };
    return res;
}
