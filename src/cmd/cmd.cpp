#include "cmd.h"
#include "cmd_add_tags.h"
#include "cmd_remove_tags.h"
#include "cmd_open_repo.h"
#include "cmd_close_repo.h"
#include "cmd_get_repos_info.h"
#include "cmd_get_file_info.h"
#include "cmd_search.h"
#include "cmd_get_repos_info.h"
#include <reggata_exceptions.h>
#include <boost/log/trivial.hpp>

Cmd::Cmd(const std::string& id, Cmd::SendResult sendResult) :
_id(id),
_sendResult(sendResult) {
}

std::unique_ptr<Cmd> Cmd::fromJson(const nlohmann::json& j, Cmd::SendResult sendResult) {
    std::string cmdStr = j.at("cmd");

    if (cmdStr == CmdAddTags::_name) {
        return Cmd::fromJson2<CmdAddTags>(j, sendResult);

    } else if (cmdStr == CmdGetFileInfo::_name) {
        return Cmd::fromJson2<CmdGetFileInfo>(j, sendResult);

    } else if (cmdStr == CmdRemoveTags::_name) {
        return Cmd::fromJson2<CmdRemoveTags>(j, sendResult);

    } else if (cmdStr == CmdSearch::_name) {
        return Cmd::fromJson2<CmdSearch>(j, sendResult);

    } else if (cmdStr == CmdOpenRepo::_name) {
        return Cmd::fromJson2<CmdOpenRepo>(j, sendResult);

    } else if (cmdStr == CmdCloseRepo::_name) {
        return Cmd::fromJson2<CmdCloseRepo>(j, sendResult);

    } else if (cmdStr == CmdGetReposInfo::_name) {
        return Cmd::fromJson2<CmdGetReposInfo>(j, sendResult);

    } else {
        throw ReggataException(std::string("Unexpected command: ") + cmdStr);
    }
}

void Cmd::sendResult(nlohmann::json& result) {
    result["id"] = _id;
    auto str = result.dump();
    _sendResult(str);
}
