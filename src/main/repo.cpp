#include "repo.h"
#include <boost/filesystem.hpp>
#include <iostream>

Repo::Repo(const std::string& rootPath, const std::string& dbPath)
: _rootPath(rootPath), _dbPath(dbPath) {

	rocksdb::Options options;
	options.create_if_missing = true;
	rocksdb::DB* db;
	rocksdb::Status status = rocksdb::DB::Open(options, _dbPath, &db);
	_db.reset(db);
	std::cout << "rocksdb status is OK=" << status.ok() << std::endl;

	for (auto&& entry : boost::filesystem::recursive_directory_iterator(_rootPath)) {
		if (entry.status().type() != boost::filesystem::file_type::directory_file) {
			continue;
		}
		createDirWatcherIfNeeded(entry.path().string());
	}
	createDirWatcherIfNeeded(rootPath);
}

Repo::~Repo() {}

void Repo::createDirWatcherIfNeeded(const std::string& dirPath) {
	if (dirPath == _dbPath) {
		std::cout << "Skipping dir " << dirPath << std::endl;
		return;
	}
	createDirWatcher(dirPath);
}

void Repo::createDirWatcher(const std::string& dirPath) {
	std::unique_ptr<Poco::DirectoryWatcher> watcher(new Poco::DirectoryWatcher(dirPath,
			  Poco::DirectoryWatcher::DW_FILTER_ENABLE_ALL, 2));
	watcher->itemAdded += Poco::delegate(this, &Repo::onFileAdded);
	watcher->itemRemoved += Poco::delegate(this, &Repo::onFileRemoved);
	watcher->itemModified += Poco::delegate(this, &Repo::onFileChanged);
	watcher->itemMovedFrom += Poco::delegate(this, &Repo::onFileMovedFrom);
	watcher->itemMovedTo += Poco::delegate(this, &Repo::onFileMovedTo);
	watcher->scanError += Poco::delegate(this, &Repo::onScanError);
	_watchers[dirPath] = std::move(watcher);
	std::cout << "DirWatcher created for " << dirPath << std::endl;
}

void Repo::destroyDirWatcherIfExists(const std::string& dirPath) {
	auto f = _watchers.find(dirPath);
	if (f != _watchers.end()) {
		_watchers.erase(f);
		std::cout << "DirWatcher destroyed for " << dirPath << std::endl;
	}
	// else silently do nothing
}

void Repo::onFileAdded(const Poco::DirectoryWatcher::DirectoryEvent& event) {
    std::cout << "Added: " << event.item.path() << std::endl;
    if (event.item.exists() && event.item.isDirectory()) {
    	createDirWatcher(event.item.path());
    }
}

void Repo::onFileRemoved(const Poco::DirectoryWatcher::DirectoryEvent& event) {
	const std::string& path = event.item.path();
	destroyDirWatcherIfExists(path);
	std::cout << "Removed: " << event.item.path() << std::endl;
}

void Repo::onFileChanged(const Poco::DirectoryWatcher::DirectoryEvent& event) {
    std::cout << "Changed: " << event.item.path() << std::endl;
}

void Repo::onFileMovedFrom(const Poco::DirectoryWatcher::DirectoryEvent& event) {
	std::cout << "MovedFrom: " << event.item.path() << std::endl;
}

void Repo::onFileMovedTo(const Poco::DirectoryWatcher::DirectoryEvent& event) {
	std::cout << "MovedTo: " << event.item.path() << std::endl;
}

void Repo::onScanError(const Poco::Exception& ex) {
	std::cout << "ScanError: " << ex.message() << std::endl;
}


