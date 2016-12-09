#pragma once
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <iostream>

using boost::asio::ip::tcp;

class Client {
	boost::asio::io_service _ios;
	boost::asio::ip::tcp::socket _socket;

public:
	Client(int port);
	virtual ~Client() = default;

	boost::system::error_code send(const std::string& message);
	std::string recv();
};
