#include "main.h"
#include "parser/Parser.h"
#include "service.h"

#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <boost/log/core.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/trivial.hpp>
#include <boost/filesystem.hpp>
using boost::filesystem::absolute;
#include <boost/shared_ptr.hpp>
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
	boost::log::add_console_log();
	BOOST_LOG_TRIVIAL(info) << "=== Reggata Daemon start ===";
	auto executable_path = absolute(argv[0]).parent_path();
	pt::ptree config;
	pt::read_json((executable_path / "reggatad.conf").string(), config);

	auto port = config.get<int>("listen_port", 9100);
	boost::shared_ptr<Service> s(new Service(port));

	for(auto &repo : config.get_child("repos")) {
		auto rootPath = repo.second.get<std::string>("root_path");
		auto dbPath = repo.second.get<std::string>("db_path", std::string(".reggata"));
		std::cout << "Found repository: " << rootPath << std::endl;
		s->open_repo(rootPath, absolute(dbPath, rootPath).string());
	}
	s->start();

	// TODO: remove this
	Parser parser;// It's just to check that flex/bison do work
	parser.parse();

	// TODO: implement a way to stop server

	return EXIT_SUCCESS;
}

// just some function to have something that can be test
#include <exception>

void GetFrobnicationInterval() {
	throw std::string(
			"InvalidOperation: frobnication interval can't be retrieved");
}

