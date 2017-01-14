#pragma once
#include "cmds.h"
#include "safe_queue.h"
#include "database.h"
#include "file_info.h"
#include <Poco/DirectoryWatcher.h>
#include <Poco/Delegate.h>
#include <boost/log/trivial.hpp>
#include <boost/filesystem.hpp>
#include <boost/uuid/random_generator.hpp>
#include <string>
#include <map>
#include <memory>

class Repo {
    std::string _rootPath; // TODO: use path class
    std::string _dbPath; // TODO: use path class
    std::unique_ptr<Database> _db;
    boost::uuids::random_generator _uuidGenerator;
    SafeQueue<std::unique_ptr<CmdRepo>> _queue;
    volatile bool _stopCalled = false;
    boost::thread _thread;
    std::map<const std::string, std::unique_ptr<Poco::DirectoryWatcher>> _watchers;
    
public:
    Repo(const std::string& rootPath, const std::string& dbPath, bool initIfNotExists = false);
    virtual ~Repo() = default;
    
    void stop();
    
    std::string rootPath() const;
    void enqueueCmd(std::unique_ptr<CmdRepo> cmd);
    
    void addTags(const boost::filesystem::path& fileAbs, const std::vector<std::string>& tags);
    void removeTags(const boost::filesystem::path& fileAbs, const std::vector<std::string>& tags);
    FileInfo getFileInfo(const boost::filesystem::path& fileAbs) const;
    
    boost::filesystem::path makeRelativePath(const boost::filesystem::path& abs) const;
    
private:
    std::string getOrCreateFileId(const boost::filesystem::path& fileRel);
    bool getFileId(const boost::filesystem::path& fileRel, std::string* fileId) const;
    std::string createFileId(const boost::filesystem::path& fileRel);
    void addTag(const std::string& fileId, const std::string& tag);
    void removeTag(const std::string& fileId, const std::string& tag);

private:
    void run();
    void start();
    
    void createDirWatcherIfNeeded(const std::string& dirPath);
    void createDirWatcher(const std::string& dirPath);
    void destroyDirWatcherIfExists(const std::string& dirPath);

    void onFileAdded(const Poco::DirectoryWatcher::DirectoryEvent& event);
    void onFileRemoved(const Poco::DirectoryWatcher::DirectoryEvent& event);
    void onFileChanged(const Poco::DirectoryWatcher::DirectoryEvent& event);
    void onFileMovedFrom(const Poco::DirectoryWatcher::DirectoryEvent& event);
    void onFileMovedTo(const Poco::DirectoryWatcher::DirectoryEvent& event);
    void onScanError(const Poco::Exception& ex);
};
