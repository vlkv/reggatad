#pragma once
#include <boost/weak_ptr.hpp>
#include <string>
#include <exception>


class ReggataException : public std::exception {
public:
	explicit ReggataException(const std::string& msg) :
		_msg(msg) {
	}

	virtual const char* what() const throw () {
		return _msg.c_str();
	}
protected:
	std::string _msg;
};

class ClientConnection;

class ConnException : public ReggataException {
	boost::weak_ptr<ClientConnection> _client;
public:
	explicit ConnException(const std::string& msg, boost::weak_ptr<ClientConnection> client) :
		ReggataException(msg),
		_client(client) {
	}

	boost::weak_ptr<ClientConnection> client() const {
		return _client;
	}
};
