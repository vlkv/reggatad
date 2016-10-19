#include "processor.h"
#include "reggata_exceptions.h"
#include "cmds.h"

Processor::Processor() {
}

void Processor::openRepo(const std::string& repoRootDir, const std::string& repoDbDir) {
	// TODO: forbid open nested repos
	BOOST_LOG_TRIVIAL(info) << "Open repo, rootDir=" << repoRootDir << " dbDir=" << repoDbDir;
	_repos.insert(Repos::value_type(repoRootDir, std::unique_ptr<Repo>(new Repo(repoRootDir, repoDbDir))));
}

void Processor::routeCmd(Cmd* cmd) {
	CmdRepo* cmdRepo = dynamic_cast<CmdRepo*>(cmd);
	if (cmdRepo != nullptr) {
		auto p = cmdRepo->path();
		auto* repo = findRepo(p);
		if (repo == nullptr) {
			throw new ReggataException(std::string("Could not find repo for path=") + p);
		}
		cmdRepo->setContext(repo);
		repo->enqueueCmd(cmdRepo);
		return;
	}

	CmdProc* cmdProc = dynamic_cast<CmdProc*>(cmd);
	if (cmdProc != nullptr) {
		cmdProc->setContext(this);
		enqueueCmd(cmdProc);
		return;
	}

	throw new ReggataException(std::string("Unknown command") + cmd->_id);
}

Repo* Processor::findRepo(const std::string& path) {
	for (Repos::iterator kv = _repos.begin(); kv != _repos.end(); ++kv) {
		if (path.find(kv->first) == 0) {
			return kv->second.get();
		}
	}
	return nullptr;
}

void Processor::enqueueCmd(CmdProc* cmd) {
	// TODO
}
