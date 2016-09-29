#include <Repo.h>
#include <boost/filesystem.hpp>
#include <iostream>

Repo::Repo(const std::string& rootPath) : _rootPath(rootPath) {

	for (auto&& entry : boost::filesystem::recursive_directory_iterator(_rootPath)) {
		if (entry.status().type() != boost::filesystem::file_type::directory_file) {
			continue;
		}
		createDirWatcher(entry.path().string());
	}
	createDirWatcher(rootPath);
}

Repo::~Repo() {}

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

void Repo::onFileAdded(const Poco::DirectoryWatcher::DirectoryEvent& event) {
	// TODO: createDirWatcher if item is a directory
    std::cout << "Added: " << event.item.path() << std::endl;
}

void Repo::onFileRemoved(const Poco::DirectoryWatcher::DirectoryEvent& event) {
	// TODO: remove dir watcher if item was a directory
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


