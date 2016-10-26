#pragma once
#include "cmd_proc.h"

struct CmdOpenRepo: public CmdProc {
	static const std::string NAME;

	std::string _rootDir;
	std::string _dbDir;
	bool _initIfNotExists;

	CmdOpenRepo(const std::string& id, Cmd::SendResult sendResult);
	virtual ~CmdOpenRepo() = default;

	static std::unique_ptr<CmdOpenRepo> fromJson(const json::json& j, SendResult sendResult);
	virtual void execute();
};
