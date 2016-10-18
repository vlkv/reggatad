#include "cmd_repo.h"
#include "processor.h"
#include "reggata_exceptions.h"

CmdRepo::CmdRepo(const std::string& id, Cmd::SendResult sendResult) :
	Cmd(id, sendResult) {
}

void CmdRepo::enqueueTo(Processor* proc) {
	auto p = path();
	auto* repo = proc->findRepo(p);
	if (repo == nullptr) {
		throw new ReggataException(std::string("Could not find repo for path=") + p);
	}
	repo->enqueueCmd(this);
}
