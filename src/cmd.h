#pragma once
#include "json.hpp"
namespace json = nlohmann;
#include <boost/assert.hpp>

#include <string>

class Cmd {
public:
	std::string _id;

	Cmd(const std::string& id);
	virtual ~Cmd() = default;

	static std::unique_ptr<Cmd> parse(const json::json& j);
};
