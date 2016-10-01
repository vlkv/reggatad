#include "main.h"
#include "parser/Parser.h"
#include "Service.h"

#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <boost/filesystem.hpp>
using boost::filesystem::absolute;

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <set>
#include <exception>
namespace pt = boost::property_tree;


// when testing, the main of gtest and this main function will collide,
// this prevents the collision, but it's an ugly hack like all ifdef branches
#ifdef UNIT_TESTS
#define MAIN not_main
#else
#define MAIN main
#endif

int MAIN(int argc, char** argv) {
  auto executable_path = absolute(argv[0]).parent_path();
  pt::ptree config;
  pt::read_json((executable_path / "reggatad.conf").string(), config);

  std::unique_ptr<Service> s(new Service());

  for(pt::ptree::value_type &repo : config.get_child("repos")) {
	  auto rootPath = repo.second.get<std::string>("root_path");
	  auto dbPath = repo.second.get<std::string>("db_path", std::string(".reggata"));
      std::cout << "Found repository: " << rootPath << std::endl;
      s->openRepo(rootPath, absolute(dbPath, rootPath).string());
  }

  Parser parser; // It's just to check that flex/bison do work
  parser.parse();

  std::string anyKey;
  std::cin >> anyKey;

  return EXIT_SUCCESS;
}


// just some function to have something that can be test
#include <exception>

void GetFrobnicationInterval() {
  throw std::string("InvalidOperation: frobnication interval can't be retrieved");
}



