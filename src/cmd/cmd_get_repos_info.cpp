#include "cmd_get_repos_info.h"
#include <status_code.h>
#include <processor.h>

CmdGetReposInfo::CmdGetReposInfo(const std::string& id, Cmd::SendResult sendResult) :
CmdProc(id, sendResult) {
}

const std::string CmdGetReposInfo::_name = "get_repos_info";

const JsonMap::ParseMap<CmdGetReposInfo> CmdGetReposInfo::_parseMap =
        JsonMap::ParseMap<CmdGetReposInfo>();

nlohmann::json CmdGetReposInfo::execute() {
    auto reposInfo = _proc->getReposInfo();
    auto riJson = nlohmann::json::array();
    for (auto ri : reposInfo) {
        riJson.push_back(ri.toJson());
    }
    nlohmann::json res = {
        {"code", StatusCode::OK},
        {"data", riJson}
    };
    return res;
}