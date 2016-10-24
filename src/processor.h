#pragma once
#include "repo.h"
#include "safe_queue.h"
#include <boost/log/trivial.hpp>

class Cmd;
class CmdRepo;
class CmdProc;

class Processor {
	typedef std::map<std::string, std::unique_ptr<Repo>> Repos; // TODO: use shared_ptr
	Repos _repos;

	SafeQueue<std::unique_ptr<CmdProc>> _queue;

public:
	Processor();
	virtual ~Processor() = default;

	void start();
	void openRepo(const std::string& repoRootDir, const std::string& repoDbDir);
	void routeCmd(Cmd* cmd);

private:
	Repo* findRepo(const std::string& path);
	void enqueueCmd(CmdProc* cmd);
};
