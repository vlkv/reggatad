#pragma once
#include "safe_queue.h"
#include "repo_info.h"
#include <boost/thread.hpp>
#include <boost/filesystem/path.hpp>

class Repo;
class Cmd;
class CmdRepo;
class CmdProc;

class Processor : public std::enable_shared_from_this<Processor> {
    typedef std::map<std::string, std::shared_ptr<Repo>> Repos;
    Repos _repos;
    SafeQueue<std::unique_ptr<CmdProc>> _queue;
    volatile bool _stopCalled = false;
    boost::thread _thread;

public:
    Processor();
    virtual ~Processor() = default;

    void start();
    void stop();
    void routeCmd(std::unique_ptr<Cmd> cmd);

public:
    void openRepo(const boost::filesystem::path& repoRootDir, 
            const boost::filesystem::path& repoDbDir, bool initIfNotExists);
    void closeRepo(const boost::filesystem::path& repoRootDir);
    std::vector<RepoInfo> getReposInfo() const;

private:
    void run();
    void enqueueCmd(std::unique_ptr<CmdProc> cmd);
    std::shared_ptr<Repo> findRepo(const boost::filesystem::path& path);
};
