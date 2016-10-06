#pragma once
#include <string>
#include <exception>

class ServiceException : public std::exception {
public:
	explicit ServiceException(const std::string& message) :
		_msg(message) {
	}

	virtual const char* what() const throw () {
		return _msg.c_str();
	}
protected:
	std::string _msg;
};
