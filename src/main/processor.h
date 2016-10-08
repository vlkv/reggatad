#include "repo.h"
#include <boost/log/trivial.hpp>

#ifndef PROCESSOR_H_
#define PROCESSOR_H_

class Processor {
	std::vector<std::unique_ptr<Repo>> _repos;

public:
	Processor();
	virtual ~Processor() = default;

	void openRepo(const std::string& repoRootPath, const std::string& repoDbPath);
};

#endif /* PROCESSOR_H_ */
