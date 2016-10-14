#pragma once
#include <boost/property_tree/ptree.hpp>

class Cmd {
public:
	Cmd();
	virtual ~Cmd() = default;

	static std::unique_ptr<Cmd> parse(const boost::property_tree::ptree& pt);
};
