#include "main.h"
#include "parser/Parser.h"
#include "service.h"
#include "processor.h"

#include <boost/log/core.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/trivial.hpp>
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
namespace pt = boost::property_tree;

#include <set>
#include <exception>
#include <memory>
#include <iostream>
#include <fstream>
#include <string>

// when testing, the main of gtest and this main function will collide,
// this prevents the collision, but it's an ugly hack like all ifdef branches
#ifdef UNIT_TESTS
#define MAIN not_main
#else
#define MAIN main
#endif

int MAIN(int argc, char** argv) {
	boost::log::add_console_log();
	BOOST_LOG_TRIVIAL(info)<< "=== Reggata Daemon start ===";
	auto executable_path = fs::absolute(argv[0]).parent_path();
	pt::ptree config;
	pt::read_json((executable_path / "reggatad.conf").string(), config);

	auto proc = std::make_shared<Processor>();
	for (auto &repo : config.get_child("repos")) {
		auto rootPath = repo.second.get<std::string>("root_path");
		auto dbPath = repo.second.get<std::string>("db_path",
				std::string(".reggata"));
		proc->openRepo(rootPath, fs::absolute(dbPath, rootPath).string());
	}

	auto port = config.get<int>("listen_port", 9100);
	std::unique_ptr<Service> s(new Service(port, proc));
	s->start();
	// TODO: implement a way to stop server

	// TODO: remove this
	Parser parser; // It's just to check that flex/bison do work
	parser.parse();

	return EXIT_SUCCESS;
}

// just some function to have something that can be test
#include <exception>

void GetFrobnicationInterval() {
	throw std::string(
			"InvalidOperation: frobnication interval can't be retrieved");
}

