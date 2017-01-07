#pragma once

#include "reggata_exceptions.h"
class Service;
#include "processor.h"
#include "cmds.h"
#include <nlohmann/json.hpp>
namespace json = nlohmann;
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/log/trivial.hpp>
#include <boost/function.hpp>
#include <iostream>
#include <sstream>
#include <memory>
#include <string>

class ClientConnection {
    int _id;
    static int _nextId;

    boost::asio::ip::tcp::socket _sock;
    boost::asio::deadline_timer _pingTimer;
    boost::asio::deadline_timer _autoCloseConnectionTimer;
    bool _autoCloseConnectionTimerIsStarted = false;

    const size_t _headerSize = 4;
    std::vector<char> _readBuffer;
    std::vector<char> _writeBuffer;

    std::shared_ptr<Processor> _proc;

    bool _pingClient;

public:
    typedef boost::shared_ptr<ClientConnection> ptr;

    ClientConnection(boost::asio::io_service& ioService, std::shared_ptr<Processor> proc, bool pingClient);
    virtual ~ClientConnection() = default;

    void start();
    void stop();

    boost::asio::ip::tcp::socket& sock();
    int id() const;

private:
    void doReadHeader();
    void doReadBody(int bodyLength);
    void onReadHeader(const boost::system::error_code& err);
    void onReadBody(const boost::system::error_code& err);
    uint32_t decodeHeader(const std::vector<char>& buf) const;
    void handleMsg(const std::string &msg);
    void handleCmdResult(const std::string& result);

    typedef boost::function<void(const boost::system::error_code &, size_t) > OnWriteHandler;
    void doWrite(const std::string &msg, OnWriteHandler onWriteHandler);
    void onCmdResultWritten(const boost::system::error_code& err, size_t bytes);

    void startPingTimer();
    void onPingTimer(const boost::system::error_code& err);
    void onPingSent(const boost::system::error_code& err, size_t bytes);
    void startAutoCloseConnectionTimer();
    void restartAutoCloseConnectionTimer();
    void onAutoCloseConnectionTimer(const boost::system::error_code& err);
};
