/*
 * Service.cpp
 *
 *  Created on: Sep 28, 2016
 *      Author: vitvlkv
 */

#include <Service.h>

Service::Service() {
}

Service::~Service() {
}


void Service::openRepo(const std::string& repoPath) {
	_repos.push_back(std::unique_ptr<Repo>(new Repo(repoPath)));
}

