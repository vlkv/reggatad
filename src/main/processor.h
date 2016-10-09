#pragma once
#include "repo.h"
#include <boost/log/trivial.hpp>

class Processor {
	std::vector<std::unique_ptr<Repo>> _repos;

public:
	Processor();
	virtual ~Processor() = default;

	void openRepo(const std::string& repoRootPath, const std::string& repoDbPath);
};
