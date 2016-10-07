#ifndef REPO_H_
#define REPO_H_

#include "Poco/DirectoryWatcher.h"
#include "Poco/Delegate.h"
#include <rocksdb/db.h>
#include <string>
#include <map>
#include <memory>

class Repo {
	std::string _rootPath;
	std::string _dbPath;
	std::unique_ptr<rocksdb::DB> _db;
	std::map<const std::string, std::unique_ptr<Poco::DirectoryWatcher>> _watchers;
public:
	Repo(const std::string& rootPath, const std::string& dbPath);
	virtual ~Repo();

private:
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

#endif /* REPO_H_ */
