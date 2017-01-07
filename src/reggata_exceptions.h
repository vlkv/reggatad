#pragma once
#include <string>
#include <exception>

class ReggataException : public std::exception {
    std::string _msg;

public:
    explicit ReggataException(const std::string& msg) :
    _msg(msg) {
    }

    virtual const char* what() const throw () {
        return _msg.c_str();
    }
    
};


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


class StatusCodeException : public ReggataException {
    int _statusCode;

public:
    explicit StatusCodeException(int statusCode, const std::string& msg) :
    ReggataException(msg),
    _statusCode(statusCode) {
    }

    int statusCode() const {
        return _statusCode;
    }
};


class ParseCmdException : public ReggataException {
    std::string _cmdId;

public:
    explicit ParseCmdException(const std::string& cmdId, const std::string& msg) :
    ReggataException(msg),
    _cmdId(cmdId) {
    }

    std::string cmdId() const {
        return _cmdId;
    }
};
