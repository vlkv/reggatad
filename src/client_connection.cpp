#include "client_connection.h"
#include "status_code.h"
#include <cmd/cmd.h>
#include <boost/format.hpp>

int ClientConnection::_nextId = 1;

ClientConnection::ClientConnection(boost::asio::io_service& ioService, std::shared_ptr<Processor> proc,
        int pingIntervalMs) :
_id(ClientConnection::_nextId++),
_sock(ioService),
_pingClient(pingIntervalMs > 0),
_pingIntervalMs(pingIntervalMs),
_pingTimer(ioService),
_autoCloseConnectionIntervalMs(int(1.8 * pingIntervalMs)), // Should be a little less than 2*pingIntervalMs
_autoCloseConnectionTimer(ioService),
_proc(proc) {
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
    _pingTimer.expires_from_now(boost::posix_time::millisec(_pingIntervalMs));
    _pingTimer.async_wait(boost::bind(&ClientConnection::onPingTimer, this, _1));
}

void ClientConnection::startAutoCloseConnectionTimer() {
    if (!_pingClient) {
        return;
    }
    if (_autoCloseConnectionTimerIsStarted) {
        return;
    }
    _autoCloseConnectionTimerIsStarted = true;

    _autoCloseConnectionTimer.expires_from_now(boost::posix_time::millisec(_autoCloseConnectionIntervalMs));
    _autoCloseConnectionTimer.async_wait(boost::bind(&ClientConnection::onAutoCloseConnectionTimer, this, _1));
}

void ClientConnection::restartAutoCloseConnectionTimer() {
    if (!_pingClient) {
        return;
    }
    if (_autoCloseConnectionTimer.expires_from_now(boost::posix_time::millisec(_autoCloseConnectionIntervalMs)) > 0) {
        _autoCloseConnectionTimer.async_wait(boost::bind(&ClientConnection::onAutoCloseConnectionTimer, this, _1));
    }
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
    try {
        handleMsg(msg);
    } catch (const ParseCmdException& ex) {
        json::json resp{
            {"id", ex.cmdId()},
            {"code", StatusCode::CLIENT_ERROR},
            {"msg", ex.what()}};
        handleCmdResult(resp.dump());
    } catch (const std::exception& ex) {
        json::json resp{
            {"code", StatusCode::CLIENT_ERROR},
            {"msg", ex.what()}};
        handleCmdResult(resp.dump());
    } catch (...) {
        json::json resp{
            {"code", StatusCode::CLIENT_ERROR},
            {"msg", "Unknown exception in onReadBody"}};
        handleCmdResult(resp.dump());
    }
    doReadHeader();
}

void ClientConnection::handleMsg(const std::string &msg) {
    auto j = json::json::parse(msg);
    auto cmdId = j.value("id", std::string()); // We'd like to put 'id' to response if it presents in case of errors
    try {
        if (_pingClient && j.count("cmd") == 0 && j.value("answer", std::string("No")) == "Yes") {
            restartAutoCloseConnectionTimer();
        } else {
            j.at("id"); // Would throw out_of_range error if 'id' is missing
            auto cmd = Cmd::fromJson(j,
                    boost::bind(&ClientConnection::handleCmdResult, this, _1));
            _proc->routeCmd(std::move(cmd));
        }
    } catch (const std::exception& ex) {
        throw ParseCmdException(cmdId, ex.what());
    } catch (...) {
        throw ParseCmdException(cmdId, "Unknown exception in handleMsg");
    }
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
    startAutoCloseConnectionTimer();
}
