#pragma once
#include "json.hpp"
namespace json = nlohmann;
#include <boost/assert.hpp>

#include <string>

struct Cmd {
	std::string _id;

	typedef std::function<void(const std::string&)> SendResult;
	SendResult _sendResult;

	Cmd(const std::string& id, SendResult sendResult);
	virtual ~Cmd() = default;

	static std::unique_ptr<Cmd> fromJson(const json::json& j, SendResult sendResult);

	virtual void execute() = 0;
};
