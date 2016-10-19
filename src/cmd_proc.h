#pragma once
#include "cmd.h"
#include "processor.h"

struct CmdProc : public Cmd {
	Processor* _proc;

	CmdProc(const std::string& id, Cmd::SendResult sendResult);
	virtual ~CmdProc() = default;

	void setContext(Processor* proc);
};
