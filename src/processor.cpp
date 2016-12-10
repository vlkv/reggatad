#include "processor.h"
#include "reggata_exceptions.h"
#include "cmds.h"

Processor::Processor() {
}

void Processor::start() {
	_thread = boost::thread(&Processor::run, this);
	// ATTN: after start is called, it's not thread-safe to call openRepo! Do not call it. Instead, enqueue a command open_repo
}

void Processor::stop() {
	BOOST_LOG_TRIVIAL(info) << "Stopping Processor...";
	_stopCalled = true;
	_thread.interrupt();
	_thread.join();
	BOOST_LOG_TRIVIAL(info) << "Processor stopped";
}

void Processor::run() {
	BOOST_LOG_TRIVIAL(info) << "Processor started";
	while (!_stopCalled) {
		try {
			auto cmd = _queue.dequeue();
			try {
				auto result = cmd->execute();
				//throw std::exception();
				cmd->sendResult(result);
			} catch (const std::exception& ex) {
				json::json result = {
					{"ok", false},
					{"reason", ex.what()}
				};
				cmd->sendResult(result);
			}
		} catch (const std::exception& ex) {
			BOOST_LOG_TRIVIAL(error) << "std::exception " << ex.what();
		} catch (...) {
			BOOST_LOG_TRIVIAL(error) << "Unexpected exception";
		}
	}
	BOOST_LOG_TRIVIAL(info) << "Processor:run exited";
}

void Processor::openRepo(const std::string& repoRootDir, const std::string& repoDbDir) {
	// TODO: forbid open nested repos
	BOOST_LOG_TRIVIAL(info) << "Open repo, rootDir=" << repoRootDir << " dbDir=" << repoDbDir;
	auto repo = std::make_shared<Repo>(repoRootDir, repoDbDir);
	_repos.insert(Repos::value_type(repoRootDir, repo));
}

void Processor::routeCmd(std::unique_ptr<Cmd> cmd) {
	auto* cmdRepo = dynamic_cast<CmdRepo*>(cmd.get());
	if (cmdRepo != nullptr) {
		std::unique_ptr<CmdRepo> cmdRepo1(cmdRepo);
		cmd.release();
		auto p = cmdRepo1->path();
		auto repo = findRepo(p);
		if (repo.use_count() == 0) {
			throw new ReggataException(std::string("Could not find repo for path=") + p);
		}
		cmdRepo1->setContext(repo);
		repo->enqueueCmd(std::move(cmdRepo1));
		return;
	}

	auto* cmdProc = dynamic_cast<CmdProc*>(cmd.get());
	if (cmdProc != nullptr) {
		std::unique_ptr<CmdProc> cmdProc1(cmdProc);
		cmd.release();
		cmdProc1->setContext(shared_from_this());
		enqueueCmd(std::move(cmdProc1));
		return;
	}

	throw new ReggataException(std::string("Unknown command") + cmd->_id);
}

std::shared_ptr<Repo> Processor::findRepo(const std::string& path) {
	for (Repos::iterator kv = _repos.begin(); kv != _repos.end(); ++kv) {
		if (path.find(kv->first) == 0) {
			return kv->second;
		}
	}
	std::shared_ptr<Repo> nullPtr;
	return nullPtr;
}

void Processor::enqueueCmd(std::unique_ptr<CmdProc> cmd) {
	_queue.enqueue(std::move(cmd));
}
