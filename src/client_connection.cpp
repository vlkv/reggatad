#include "client_connection.h"
#include <boost/format.hpp>

int ClientConnection::_nextId = 1;

ClientConnection::ClientConnection(boost::asio::io_service& io_service, std::shared_ptr<Processor> proc,
        bool pingClient) :
_id(ClientConnection::_nextId++),
_sock(io_service),
_pingTimer(io_service),
_autoCloseConnectionTimer(io_service),
_proc(proc),
_pingClient(pingClient) {
    BOOST_LOG_TRIVIAL(info) << "Created ClientConnection id=" << _id;
}

void ClientConnection::start() {
    startPingTimer();
    doReadHeader();
}

void ClientConnection::stop() {
    BOOST_LOG_TRIVIAL(info) << "Stopping ClientConnection id=" << _id;
    _pingTimer.cancel();
    _autoCloseConnectionTimer.cancel();

    boost::system::error_code ec;
    _sock.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    if (ec) {
        BOOST_LOG_TRIVIAL(error) << "Could not shutdown the socket, client id=" << _id;
    }
    _sock.close();
}

void ClientConnection::startPingTimer() {
    if (!_pingClient) {
        return;
    }
    _pingTimer.expires_from_now(boost::posix_time::seconds(5)); // TODO: parametrize timeout
    _pingTimer.async_wait(boost::bind(&ClientConnection::onPingTimer, this, _1));
}

void ClientConnection::startAutoCloseConnectionTimer() {
    if (!_pingClient) {
        return;
    }
    _autoCloseConnectionTimer.expires_from_now(boost::posix_time::seconds(9)); // Should be a little less than 2*ping_timeout
    _autoCloseConnectionTimer.async_wait(boost::bind(&ClientConnection::onAutoCloseConnectionTimer, this, _1));
}

void ClientConnection::onPingTimer(const boost::system::error_code& err) {
    if (err == boost::system::errc::operation_canceled) {
        BOOST_LOG_TRIVIAL(debug) << "onPingTimer cancelled";
        return;
    }
    if (err) {
        throw ConnException((boost::format("Ping timer error: %1% %2% client id=%3%")
                % err % err.message() % _id).str(), _id);
    }
    BOOST_LOG_TRIVIAL(debug) << "onPingTimer";
    json::json pingMsg{
        {"question", "Are you alive? Respond with {\"answer\": \"Yes\"} otherwise server would close the connection"}};
    doWrite(pingMsg.dump(), boost::bind(&ClientConnection::onPingSent, this, _1, _2));
}

void ClientConnection::onAutoCloseConnectionTimer(const boost::system::error_code& err) {
    if (err == boost::system::errc::operation_canceled) {
        BOOST_LOG_TRIVIAL(debug) << "onAutoCloseConnectionTimer cancelled";
        return;
    }
    if (err) {
        throw ConnException((boost::format("Auto close connection timer error: %1% %2% client id=%3%")
                % err % err.message() % _id).str(), _id);
    }
    throw ConnException((boost::format("Closing connection with client id=%1%, because it doesn't respond to pings")
            % _id).str(), _id);
}

boost::asio::ip::tcp::socket& ClientConnection::sock() {
    return _sock;
}

int ClientConnection::id() const {
    return _id;
}

void ClientConnection::doReadHeader() {
    BOOST_LOG_TRIVIAL(debug) << "doReadHeader";
    _readBuffer.resize(_headerSize);
    boost::asio::async_read(_sock, boost::asio::buffer(_readBuffer),
            boost::bind(&ClientConnection::onReadHeader, this, _1));
}

void ClientConnection::onReadHeader(const boost::system::error_code& err) {
    if (err == boost::system::errc::operation_canceled) {
        BOOST_LOG_TRIVIAL(debug) << "Read message header cancelled";
        return;
    }
    if (err) {
        throw ConnException((boost::format("Could not read message header, error: %1% %2% client id=%3%")
                % err % err.message() % _id).str(), _id);
    }
    auto bodySize = decodeHeader(_readBuffer);
    doReadBody(bodySize);
}

uint32_t ClientConnection::decodeHeader(const std::vector<char>& buf) const {
    if (buf.size() != _headerSize) {
        throw ConnException((boost::format("Bad header size %1% should be equal to %2% client id=%3%")
                % buf.size() % _headerSize % _id).str(), _id);
    }
    uint32_t msgSize = 0;
    char* dst = (char*) &msgSize;
    for (size_t i = 0; i < _headerSize; ++i) {
        dst[i] = buf[i];
    }
    return msgSize;
}

void ClientConnection::doReadBody(int bodySize) {
    BOOST_LOG_TRIVIAL(debug) << "doReadBody";
    _readBuffer.resize(bodySize);
    boost::asio::async_read(_sock, boost::asio::buffer(_readBuffer),
            boost::bind(&ClientConnection::onReadBody, this, _1));
}

void ClientConnection::onReadBody(const boost::system::error_code& err) {
    if (err == boost::system::errc::operation_canceled) {
        BOOST_LOG_TRIVIAL(debug) << "Read message body cancelled";
        return;
    }
    if (err) {
        throw ConnException((boost::format("Could not read message body, error: %1% %2% client id=%3%")
                % err % err.message() % _id).str(), _id);
    }
    std::string msg(_readBuffer.begin(), _readBuffer.end());
    BOOST_LOG_TRIVIAL(info) << "Received from client id=" << _id << " msg: " << msg;
    handleMsg(msg);
}

void ClientConnection::handleMsg(const std::string &msg) {
    auto j = json::json::parse(msg);

    if (_pingClient && j.find("cmd") == j.end() && j.value("answer", std::string("No")) == "Yes") {
        if (_autoCloseConnectionTimer.expires_from_now(boost::posix_time::seconds(10)) > 0) {
            _autoCloseConnectionTimer.async_wait(boost::bind(&ClientConnection::onAutoCloseConnectionTimer, this, _1));
        }
    } else {
        auto cmd = Cmd::fromJson(j, // TODO: send CLIENT_ERROR if command has bad format
                boost::bind(&ClientConnection::handleCmdResult, this, _1));
        _proc->routeCmd(std::move(cmd));
    }
    doReadHeader();
}

void ClientConnection::handleCmdResult(const std::string& result) {
    doWrite(result, boost::bind(&ClientConnection::onCmdResultWritten, this, _1, _2));
}

void ClientConnection::onCmdResultWritten(const boost::system::error_code& err, size_t bytes) {
    if (err) {
        throw ConnException((boost::format("Could not send cmd result, error: %1% %2% client id=%3%")
                % err % err.message() % _id).str(), _id);
    }
}

void ClientConnection::doWrite(const std::string &msg, OnWriteHandler onWriteHandler) {
    if (!_sock.is_open()) {
        return;
    }
    BOOST_LOG_TRIVIAL(info) << "Sending to client id=" << _id << " msg: " << msg;
    _writeBuffer.resize(4 + msg.size());
    uint32_t header = msg.size();
    std::copy((char*) &header, (char*) &header + 4, _writeBuffer.begin());
    std::copy(msg.begin(), msg.end(), _writeBuffer.begin() + 4);
    boost::asio::async_write(_sock, boost::asio::buffer(_writeBuffer), onWriteHandler);
}

void ClientConnection::onPingSent(const boost::system::error_code& err, size_t bytes) {
    if (err) {
        throw ConnException((boost::format("Could not send ping, error: %1% %2% client id=%3%")
                % err % err.message() % _id).str(), _id);
    }
    startPingTimer();
    if (!_autoCloseConnectionTimerIsStarted) {
        startAutoCloseConnectionTimer();
        _autoCloseConnectionTimerIsStarted = true;
    }
}
