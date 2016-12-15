#include "application.h"
#include "cmd_open_repo.h"

Application::Application(int port, bool pingClients = true) :
_proc(new Processor()),
_service(new Service(port, _proc, pingClients)) {
}

void Application::openRepo(const std::string& repoPath, const std::string& dbPath) {
    std::unique_ptr<CmdOpenRepo> cmd(new CmdOpenRepo("_", [](const std::string & result) {
    }));
    // TODO: init all fields in ctor
    cmd->_dbDir = repoPath;
    cmd->_rootDir = repoPath;
    cmd->_initIfNotExists = false;
    _proc->routeCmd(std::unique_ptr<Cmd>(cmd.release()));
}

void Application::start() {
    _proc->start();
    _service->startListenPort();
}

void Application::stop() {
    BOOST_LOG_TRIVIAL(info) << "Application stop";
    _service->stopAsync();
    _proc->stop();
}

