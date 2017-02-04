#include "cmd_search.h"
#include <reggata_exceptions.h>
#include <status_code.h>
#include <repo.h>
#include <parser/parser.h>
#include <lang/node.h>
#include <boost/assign.hpp>
#include <boost/format.hpp>
#include <sstream>

CmdSearch::CmdSearch(const std::string& id, Cmd::SendResult sendResult) :
CmdRepo(id, sendResult) {
}

const std::string CmdSearch::_name = "search";

const JsonMap::ParseMap<CmdSearch> CmdSearch::_parseMap = boost::assign::list_of
        (JsonMap::mapValue("dir", &CmdSearch::_dir))
(JsonMap::mapValue("query", &CmdSearch::_query));

boost::filesystem::path CmdSearch::path() const {
    return _dir;
}

nlohmann::json CmdSearch::execute() {
    boost::filesystem::path dirPath(_dir);
    if (!dirPath.is_absolute()) {
        throw StatusCodeException(StatusCode::CLIENT_ERROR,
                boost::format("dir=%1% must be an absolute path") % _dir);
    }

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
        throw StatusCodeException(StatusCode::CLIENT_ERROR, msg);
    }
    auto relDir = _repo->makeRelativePath(dirPath);
    auto fileIds = tree->findFileIdsIn(_repo, relDir);
    auto fileInfos = _repo->getFileInfos(fileIds);
    nlohmann::json data = nlohmann::json::array();
    for (auto finfo : fileInfos) {
        data.push_back(finfo.toJson());
    }
    nlohmann::json res = {
        {"code", StatusCode::OK},
        {"data", data}
    };
    return res;
}
