#include "cmd_search.h"
#include "reggata_exceptions.h"
#include <status_code.h>
#include <repo.h>
#include <parser/parser.h>
#include <lang/node.h>
#include <boost/assign.hpp>
#include <boost/format/format_fwd.hpp>
#include <sstream>

CmdSearch::CmdSearch(const std::string& id, Cmd::SendResult sendResult) :
CmdRepo(id, sendResult) {
}

const std::string CmdSearch::_name = "cmd_search";

const JsonMap::ParseMap<CmdSearch> CmdSearch::_parseMap = boost::assign::list_of
        (JsonMap::mapValue("dir", &CmdSearch::_dir))
(JsonMap::mapValue("query", &CmdSearch::_query));

std::string CmdSearch::path() const {
    return _dir;
}

json::json CmdSearch::execute() {
    std::istringstream iss(_query);
    std::ostringstream oss;
    Parser p(iss, oss);
    p.parse();
    auto tree = p.getTree();
    auto errors = p.getErrors();
    if (tree.use_count() == 0 || errors.size() > 0) {
        auto msg = (boost::format("Failed to parse query '%1%'") % _query).str();
        for (auto err : errors) {
            msg = msg + "; ERROR: " + err;
        }
        throw StatusCodeException(StatusCode::CLIENT_ERROR, msg.str());
    }
    auto relDir = _repo->makeRelativePath(_dir);
    auto fileIds = tree->findFileIdsIn(_repo, relDir);
    // TODO: convert fileIds to collection of FileInfos
    // TODO: send results

    return json::json::object();
}