/*
 * processor.cpp
 *
 *  Created on: Oct 7, 2016
 *      Author: vitvlkv
 */

#include <processor.h>

Processor::Processor() {
	// TODO Auto-generated constructor stub

}


void Processor::openRepo(const std::string& repoRootPath, const std::string& repoDbPath) {
	_repos.push_back(std::unique_ptr<Repo>(new Repo(repoRootPath, repoDbPath)));
}
