#pragma once
#include "repo.h"
#include "cmds.h"
#include <boost/log/trivial.hpp>

class Processor {
	typedef std::map<std::string, std::unique_ptr<Repo>> Repos;
	Repos _repos;

public:
	Processor();
	virtual ~Processor() = default;

	void openRepo(const std::string& repoRootDir, const std::string& repoDbDir);
	void routeCmd(Cmd* cmd);
	Repo* findRepo(const std::string& path);
};
