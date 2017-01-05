#include "cmd_open_repo.h"
#include "status_code.h"
#include <processor.h>
#include <json_map.h>
#include <boost/assign.hpp>

CmdOpenRepo::CmdOpenRepo(const std::string& id, Cmd::SendResult sendResult) :
CmdProc(id, sendResult) {
}

const std::string CmdOpenRepo::_name = "open_repo";

const JsonMap::ParseMap<CmdOpenRepo> CmdOpenRepo::_parseMap = boost::assign::list_of
        (JsonMap::mapValue("root_dir", &CmdOpenRepo::_rootDir))
(JsonMap::mapValue("db_dir", &CmdOpenRepo::_dbDir)) // TODO: use default if db_dir is not given
(JsonMap::mapValue("init_if_not_exists", &CmdOpenRepo::_initIfNotExists));

json::json CmdOpenRepo::execute() {
    _proc->openRepo(_rootDir, _dbDir, _initIfNotExists);
    json::json res = {
        {"code", StatusCode::OK}
    };
    return res;
}
