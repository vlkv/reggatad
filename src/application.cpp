#include "application.h"

Application::Application(int port) :
	_proc(new Processor()),
	_service(new Service(port, _proc)) {
}

void Application::openRepo(const std::string& repoPath, const std::string& dbPath) {
	_proc->openRepo(repoPath, dbPath);
}

void Application::start() {
	_service->start();
}

