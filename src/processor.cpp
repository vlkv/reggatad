/*
 * processor.cpp
 *
 *  Created on: Oct 7, 2016
 *      Author: vitvlkv
 */

#include "processor.h"

Processor::Processor() {
	// TODO Auto-generated constructor stub

}


void Processor::openRepo(const std::string& repoRootPath, const std::string& repoDbPath) {
	BOOST_LOG_TRIVIAL(info) << "Open repo, path=" << repoRootPath << " dbPath=" << repoDbPath;
	_repos.insert(Repos::value_type(repoRootPath, std::unique_ptr<Repo>(new Repo(repoRootPath, repoDbPath))));
}
