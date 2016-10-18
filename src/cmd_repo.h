#pragma once
#include "cmd.h"

class Processor;

struct CmdRepo : public Cmd {
	CmdRepo(const std::string& id, Cmd::SendResult sendResult);
	virtual ~CmdRepo() = default;

	virtual std::string path() const = 0;
	virtual void enqueueTo(Processor* proc);
};
