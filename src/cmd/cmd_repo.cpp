#include "cmd_repo.h"
#include <processor.h>
#include <repo.h>
#include <reggata_exceptions.h>

CmdRepo::CmdRepo(const std::string& id, Cmd::SendResult sendResult) :
	Cmd(id, sendResult) {
}

void CmdRepo::setContext(std::shared_ptr<Repo> repo) {
	_repo = repo;
}
