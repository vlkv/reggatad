#pragma once
#include "repo.h"
#include "safe_queue.h"
#include <boost/log/trivial.hpp>

class Cmd;
class CmdRepo;
class CmdProc;

class Processor : public std::enable_shared_from_this<Processor> {
	typedef std::map<std::string, std::shared_ptr<Repo>> Repos;
	Repos _repos;

	SafeQueue<std::unique_ptr<CmdProc>> _queue;

public:
	Processor();
	virtual ~Processor() = default;

	void start();
	void openRepo(const std::string& repoRootDir, const std::string& repoDbDir);
	void routeCmd(std::unique_ptr<Cmd> cmd);

private:
	std::shared_ptr<Repo> findRepo(const std::string& path);
	void enqueueCmd(std::unique_ptr<CmdProc> cmd);
};
