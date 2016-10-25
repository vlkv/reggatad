#include "cmd_proc.h"
#include "processor.h"

CmdProc::CmdProc(const std::string& id, Cmd::SendResult sendResult) :
	Cmd(id, sendResult) {
}

void CmdProc::setContext(std::shared_ptr<Processor> proc) {
	_proc = proc;
}

