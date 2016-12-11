#include "repo.h"
#include "reggata_exceptions.h"
#include <boost/filesystem.hpp>
#include <iostream>

Repo::Repo(const std::string& rootPath, const std::string& dbPath, bool initIfNotExists=false)
: _rootPath(rootPath), _dbPath(dbPath) {

    if (initIfNotExists && !boost::filesystem::exists(rootPath)) {
        auto ok = boost::filesystem::create_directories(rootPath);
        if (!ok) {
            throw ReggataException(std::string("Could not create directory ") + rootPath);
        }
    }
    
	rocksdb::Options options;
	options.create_if_missing = initIfNotExists;
	rocksdb::DB* db;
	rocksdb::Status status = rocksdb::DB::Open(options, _dbPath, &db);
	_db.reset(db);
	BOOST_LOG_TRIVIAL(debug) << "rocksdb status is OK=" << status.ok();

	for (auto&& entry : boost::filesystem::recursive_directory_iterator(_rootPath)) {
		if (entry.status().type() != boost::filesystem::file_type::directory_file) {
			continue;
		}
		createDirWatcherIfNeeded(entry.path().string());
	}
	createDirWatcherIfNeeded(rootPath);
}

void Repo::start() {
	// TODO: start a thread that reads cmds from _queue and executes them...
}

std::string Repo::rootPath() const {
	return _rootPath;
}

void Repo::enqueueCmd(std::unique_ptr<CmdRepo> cmd) {
	_queue.enqueue(std::move(cmd));
}

void Repo::createDirWatcherIfNeeded(const std::string& dirPath) {
	if (dirPath == _dbPath) {
		BOOST_LOG_TRIVIAL(debug) << "Skipping dir " << dirPath;
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
	BOOST_LOG_TRIVIAL(debug) << "DirWatcher created for " << dirPath;
}

void Repo::destroyDirWatcherIfExists(const std::string& dirPath) {
	auto f = _watchers.find(dirPath);
	if (f != _watchers.end()) {
		_watchers.erase(f);
		BOOST_LOG_TRIVIAL(debug) << "DirWatcher destroyed for " << dirPath;
	}
	// else silently do nothing
}

void Repo::onFileAdded(const Poco::DirectoryWatcher::DirectoryEvent& event) {
	BOOST_LOG_TRIVIAL(debug) << "Added: " << event.item.path();
    if (event.item.exists() && event.item.isDirectory()) {
    	createDirWatcher(event.item.path());
    }
}

void Repo::onFileRemoved(const Poco::DirectoryWatcher::DirectoryEvent& event) {
	const std::string& path = event.item.path();
	destroyDirWatcherIfExists(path);
	BOOST_LOG_TRIVIAL(debug) << "Removed: " << event.item.path();
}

void Repo::onFileChanged(const Poco::DirectoryWatcher::DirectoryEvent& event) {
	BOOST_LOG_TRIVIAL(debug) << "Changed: " << event.item.path();
}

void Repo::onFileMovedFrom(const Poco::DirectoryWatcher::DirectoryEvent& event) {
	BOOST_LOG_TRIVIAL(debug) << "MovedFrom: " << event.item.path();
}

void Repo::onFileMovedTo(const Poco::DirectoryWatcher::DirectoryEvent& event) {
	BOOST_LOG_TRIVIAL(debug) << "MovedTo: " << event.item.path();
}

void Repo::onScanError(const Poco::Exception& ex) {
	BOOST_LOG_TRIVIAL(debug) << "ScanError: " << ex.message();
}


