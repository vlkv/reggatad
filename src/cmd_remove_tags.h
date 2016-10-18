#pragma once
#include "cmd.h"

class Processor;

struct CmdRemoveTags: public Cmd { // TODO: inherit from CmdRepo
	static const std::string NAME;

	std::string _file;
	std::vector<std::string> _tags;

	CmdRemoveTags(const std::string& id, Cmd::SendResult sendResult);
	virtual ~CmdRemoveTags() = default;

	static CmdRemoveTags* fromJson(const json::json& j, Cmd::SendResult sendResult);

	virtual void enqueueTo(Processor* proc);
	virtual void execute();
};
