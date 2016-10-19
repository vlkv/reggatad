#pragma once
#include "repo.h"
#include <boost/log/trivial.hpp>

class Cmd;
class CmdRepo;
class CmdProc;

class Processor {
	typedef std::map<std::string, std::unique_ptr<Repo>> Repos;
	Repos _repos;

public:
	Processor();
	virtual ~Processor() = default;

	void openRepo(const std::string& repoRootDir, const std::string& repoDbDir);
	void routeCmd(Cmd* cmd);

private:
	Repo* findRepo(const std::string& path);
	void enqueueCmd(CmdProc* cmd);
};
