#include "cmd.h"
#include "cmd_add_tags.h"
#include "cmd_remove_tags.h"
#include "cmd_open_repo.h"
#include <reggata_exceptions.h>

#include <boost/log/trivial.hpp>

Cmd::Cmd(const std::string& id, Cmd::SendResult sendResult) :
_id(id),
_sendResult(sendResult) {
}

std::unique_ptr<Cmd> Cmd::fromJson(const json::json& j, Cmd::SendResult sendResult) {
    BOOST_LOG_TRIVIAL(debug) << "Cmd::fromJson";
    std::string cmdStr = j["cmd"];
    if (cmdStr == CmdAddTags::NAME) {
        return Cmd::fromJson2<CmdAddTags>(j, sendResult);

    } else if (cmdStr == CmdRemoveTags::NAME) {
        return Cmd::fromJson2<CmdRemoveTags>(j, sendResult);

    } else if (cmdStr == CmdOpenRepo::NAME) {
        return Cmd::fromJson2<CmdOpenRepo>(j, sendResult);

    } else {
        throw ReggataException(std::string("Unexpected command: ") + cmdStr);
    }
}

void Cmd::sendResult(json::json& result) {
    result["id"] = _id;
    auto str = result.dump();
    _sendResult(str);
}
