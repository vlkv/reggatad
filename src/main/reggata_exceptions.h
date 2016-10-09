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
	int _clientId;
public:
	explicit ConnException(const std::string& msg, int clientId) :
		ReggataException(msg),
		_clientId(clientId) {
	}

	int clientId() const {
		return _clientId;
	}
};
