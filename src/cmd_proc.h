#pragma once
#include "cmd.h"
class Processor;

struct CmdProc : public Cmd {
	Processor* _proc; // TODO: use shared_ptr

	CmdProc(const std::string& id, Cmd::SendResult sendResult);
	virtual ~CmdProc() = default;

	void setContext(Processor* proc);
};
