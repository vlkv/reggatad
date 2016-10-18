#include "processor.h"

Processor::Processor() {
}

void Processor::openRepo(const std::string& repoRootDir, const std::string& repoDbDir) {
	// TODO: forbid open nested repos
	BOOST_LOG_TRIVIAL(info) << "Open repo, rootDir=" << repoRootDir << " dbDir=" << repoDbDir;
	_repos.insert(Repos::value_type(repoRootDir, std::unique_ptr<Repo>(new Repo(repoRootDir, repoDbDir))));
}

void Processor::routeCmd(Cmd* cmd) {
	cmd->enqueueTo(this);
}

Repo* Processor::findRepo(const std::string& path) {
	for (Repos::iterator kv = _repos.begin(); kv != _repos.end(); ++kv) {
		if (path.find(kv->first) == 0) {
			return kv->second.get();
		}
	}
	return nullptr;
}
