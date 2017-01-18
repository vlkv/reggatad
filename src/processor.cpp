#include "processor.h"
#include "repo.h"
#include "reggata_exceptions.h"
#include "status_code.h"
#include <cmd/cmd_proc.h>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

Processor::Processor() {
}

void Processor::start() {
    _thread = boost::thread(&Processor::run, this);
}

void Processor::stop() {
    BOOST_LOG_TRIVIAL(info) << "Stopping Processor...";
    _stopCalled = true;
    for (auto r = _repos.begin(); r != _repos.end(); ++r) {
        r->second->stop();
    }
    _thread.interrupt();
    _thread.join();
    BOOST_LOG_TRIVIAL(info) << "Processor stopped";
}

void Processor::run() {
    BOOST_LOG_TRIVIAL(info) << "Processor started";
    while (!_stopCalled && !_thread.interruption_requested()) {
        try {
            auto cmd = _queue.dequeue();
            json::json result;
            try {
                result = cmd->execute();
            } catch (const StatusCodeException& ex) {
                result = {
                    {"code", ex.statusCode()},
                    {"msg", ex.what()}
                };
            } catch (const std::exception& ex) {
                result = {
                    {"code", StatusCode::SERVER_ERROR},
                    {"msg", ex.what()}
                };
            }
            cmd->sendResult(result);
        } catch (const boost::thread_interrupted& ex) {
            break;
        } catch (const std::exception& ex) {
            BOOST_LOG_TRIVIAL(error) << "std::exception " << ex.what();
        } catch (...) {
            BOOST_LOG_TRIVIAL(error) << "Unexpected exception in Processor::run";
        }
    }
    BOOST_LOG_TRIVIAL(info) << "Processor:run exited";
}

void Processor::openRepo(const boost::filesystem::path& repoRootDir, const boost::filesystem::path& repoDbDir, bool initIfNotExists = false) {
    // TODO: forbid open nested repos
    BOOST_LOG_TRIVIAL(info) << "Open repo, rootDir=" << repoRootDir << " dbDir=" << repoDbDir;
    auto repo = std::make_shared<Repo>(repoRootDir, repoDbDir, initIfNotExists);
    _repos.insert(Repos::value_type(repoRootDir.string(), repo));
}

void Processor::routeCmd(std::unique_ptr<Cmd> cmd) {
    auto* cmdRepo = dynamic_cast<CmdRepo*> (cmd.get());
    if (cmdRepo != nullptr) {
        std::unique_ptr<CmdRepo> cmdRepo1(cmdRepo);
        cmd.release();
        auto p = cmdRepo1->path();
        auto repo = findRepo(p);
        if (repo.use_count() == 0) {
            throw ReggataException((boost::format("Could not find repo for path %1%") % p).str());
        }
        cmdRepo1->setContext(repo);
        repo->enqueueCmd(std::move(cmdRepo1));
        return;
    }

    auto* cmdProc = dynamic_cast<CmdProc*> (cmd.get());
    if (cmdProc != nullptr) {
        std::unique_ptr<CmdProc> cmdProc1(cmdProc);
        cmd.release();
        cmdProc1->setContext(shared_from_this());
        enqueueCmd(std::move(cmdProc1));
        return;
    }

    throw ReggataException(std::string("Unknown command") + cmd->_id);
}

std::shared_ptr<Repo> Processor::findRepo(const boost::filesystem::path& path) {
    auto pathStr = path.string();
    for (Repos::iterator kv = _repos.begin(); kv != _repos.end(); ++kv) {
        if (pathStr.find(kv->first) == 0) {
            return kv->second;
        }
    }
    std::shared_ptr<Repo> nullPtr;
    return nullPtr;
}

void Processor::enqueueCmd(std::unique_ptr<CmdProc> cmd) {
    _queue.enqueue(std::move(cmd));
}
