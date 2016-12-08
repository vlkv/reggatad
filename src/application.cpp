#include "application.h"

Application::Application(int port, bool pingClients=true) :
	_proc(new Processor()),
	_service(new Service(port, _proc, pingClients)) {
}

void Application::openRepo(const std::string& repoPath, const std::string& dbPath) {
	_proc->openRepo(repoPath, dbPath);
}

void Application::start() {
	_proc->start();
	_service->startListenPort();
}

void Application::stop() {
	// TODO
}

