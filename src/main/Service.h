#ifndef SERVICE_H_
#define SERVICE_H_

#include "Repo.h"

#include <vector>
#include <memory>
#include <string>

class Service {

	std::vector<std::unique_ptr<Repo>> _repos;

public:
	Service();
	virtual ~Service();

	void openRepo(const std::string& repoRootPath, const std::string& repoDbPath);
};

#endif /* SERVICE_H_ */
