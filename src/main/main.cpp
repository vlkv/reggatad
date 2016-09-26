#include "main.h"
#include "parser/Parser.h"

#include <memory>
#include <iostream>
#include <fstream>
#include <string>
// there are other ways of getting a path relative to the executable, but this
// one is part of the STL soon (if you can talk about soon in c++ development)
#include <boost/filesystem.hpp>
using boost::filesystem::absolute;

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <set>
#include <exception>
namespace pt = boost::property_tree;

#include "Poco/DirectoryWatcher.h"
#include "Poco/Delegate.h"

// when testing, the main of gtest and this main function will collide,
// this prevents the collision, but it's an ugly hack like all ifdef branches
#ifdef UNIT_TESTS
#define MAIN not_main
#else
#define MAIN main
#endif





class Client {
public:
  void onFileAdded(const Poco::DirectoryWatcher::DirectoryEvent& event);
  void onFileChanged(const Poco::DirectoryWatcher::DirectoryEvent& event);
};

void Client::onFileAdded(const Poco::DirectoryWatcher::DirectoryEvent& event) {
    std::cout << "Added: " << event.item.path() << std::endl;
}

void Client::onFileChanged(const Poco::DirectoryWatcher::DirectoryEvent& event) {
    std::cout << "Changed: " << event.item.path() << std::endl;
}


int MAIN(int argc, char** argv) {
  // this way you can have a path relative to the executable
  // the absolute also gets rid of symlinks, so it should work pretty stable
  auto executable_path = absolute(argv[0]).parent_path();
  pt::ptree conf;
  pt::read_json((executable_path / "reggatad.conf").string(), conf);
  auto key = conf.get<std::string>("key");
  std::cout << "key value is 2 " << key << std::endl;

  BOOST_FOREACH(pt::ptree::value_type &v, conf.get_child("repo_roots")) {
      auto repo_root = v.second.data();
      std::cout << "repo_root: " << repo_root << std::endl;
  }

  Client* c = new Client();
  Poco::DirectoryWatcher* watcher = new Poco::DirectoryWatcher(std::string("~"),
		  Poco::DirectoryWatcher::DW_FILTER_ENABLE_ALL, 2);
  watcher->itemAdded += Poco::delegate(c, &Client::onFileAdded);
  watcher->itemModified += Poco::delegate(c, &Client::onFileChanged);

  // std::string anyKey;
  // std::cin >> anyKey;

  delete watcher;
  delete c;

  Parser parser;
  parser.parse();


  return EXIT_SUCCESS;
}


// just some function to have something that can be test
#include <exception>

void GetFrobnicationInterval() {
  throw std::string("InvalidOperation: frobnication interval can't be retrieved");
}



