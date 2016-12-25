#include "client_connection.h"

int ClientConnection::_next_id = 1;

ClientConnection::ClientConnection(boost::asio::io_service& io_service, std::shared_ptr<Processor> proc,
        bool pingClient) :
_id(ClientConnection::_next_id++),
_sock(io_service),
_pingTimer(io_service),
_proc(proc),
_pingClient(pingClient) {
}

void ClientConnection::start() {
    startPingTimer();
    doReadHeader();
}

void ClientConnection::stop() {
    BOOST_LOG_TRIVIAL(info) << "Stopping ClientConnection id=" << _id;
    _pingTimer.cancel();
    _sock.close();
}

void ClientConnection::onPingTimer(const boost::system::error_code& err) {
    if (err) {
        BOOST_LOG_TRIVIAL(error) << "onPingTimer error=" << err << " " << err.message();
        startPingTimer();
        return;
    }
    BOOST_LOG_TRIVIAL(debug) << "onPingTimer";
    json::json pingMsg{
        {"cmd", "ping"}};
    doWrite(pingMsg.dump(), boost::bind(&ClientConnection::onPingSent, this, _1, _2));
}

void ClientConnection::startPingTimer() {
    if (!_pingClient) {
        return;
    }
    _pingTimer.expires_from_now(boost::posix_time::seconds(5));
    _pingTimer.async_wait(boost::bind(&ClientConnection::onPingTimer, this, _1));
}

boost::asio::ip::tcp::socket& ClientConnection::sock() {
    return _sock;
}

int ClientConnection::id() const {
    return _id;
}

void ClientConnection::doReadHeader() {
    BOOST_LOG_TRIVIAL(debug) << "doReadHeader";
    _read_buffer.resize(HEADER_SIZE);
    boost::asio::async_read(_sock, boost::asio::buffer(_read_buffer),
            boost::bind(&ClientConnection::onReadHeader, this, _1));
}

void ClientConnection::onReadHeader(const boost::system::error_code& err) {
    if (err) {
        std::ostringstream oss;
        oss << "onReadHeader error=" << err << " " << err.message();
        BOOST_LOG_TRIVIAL(error) << oss.str();
        throw ConnException(oss.str(), _id);
    }
    auto bodySize = decodeHeader(_read_buffer);
    BOOST_LOG_TRIVIAL(debug) << "body size is " << bodySize;
    doReadBody(bodySize);
}

uint32_t ClientConnection::decodeHeader(const std::vector<char>& buf) const {
    if (buf.size() != HEADER_SIZE) {
        std::ostringstream ost;
        ost << "Bad buffer size=" << buf.size() << " should be equal to " << HEADER_SIZE;
        throw ConnException(ost.str(), _id);
    }
    uint32_t msgSize = 0;
    char* dst = (char*) &msgSize;
    for (size_t i = 0; i < HEADER_SIZE; ++i) {
        dst[i] = buf[i];
    }
    return msgSize;
}

void ClientConnection::doReadBody(int bodySize) {
    BOOST_LOG_TRIVIAL(debug) << "doReadBody";
    _read_buffer.resize(bodySize);
    boost::asio::async_read(_sock, boost::asio::buffer(_read_buffer),
            boost::bind(&ClientConnection::onReadBody, this, _1));
}

void ClientConnection::onReadBody(const boost::system::error_code& err) {
    if (err) {
        BOOST_LOG_TRIVIAL(error) << "onReadBody error=" << err << " " << err.message();
        doReadHeader();
        return;
    }
    std::string msg(_read_buffer.begin(), _read_buffer.end());
    BOOST_LOG_TRIVIAL(info) << "Received from client id=" << _id << " msg: " << msg;
    handleMsg(msg);
}

void ClientConnection::handleMsg(const std::string &msg) {
    BOOST_LOG_TRIVIAL(debug) << "Received msg: " << msg;
    auto j = json::json::parse(msg);
    auto cmd = Cmd::fromJson(j,
            boost::bind(&ClientConnection::handleCmdResult, this, _1));
    _proc->routeCmd(std::move(cmd));
    doReadHeader();
}

void ClientConnection::handleCmdResult(const std::string& result) {
    doWrite(result, boost::bind(&ClientConnection::onCmdResultWritten, this, _1, _2));
}

void ClientConnection::onCmdResultWritten(const boost::system::error_code& err, size_t bytes) {
    if (err) {
        std::ostringstream oss;
        oss << "Could not send cmd result, error: " << err << " " << err.message() << " client id=" << _id;
        throw ConnException(oss.str(), _id);
    }
}

void ClientConnection::doWrite(const std::string &msg, OnWriteHandler onWriteHandler) {
    BOOST_LOG_TRIVIAL(info) << "Sending to client id=" << _id << " msg: " << msg;
    _write_buffer.resize(4 + msg.size());
    uint32_t header = msg.size();
    std::copy((char*) &header, (char*) &header + 4, _write_buffer.begin());
    std::copy(msg.begin(), msg.end(), _write_buffer.begin() + 4);
    boost::asio::async_write(_sock, boost::asio::buffer(_write_buffer), onWriteHandler);
}

void ClientConnection::onPingSent(const boost::system::error_code& err, size_t bytes) {
    if (err) {
        std::ostringstream oss;
        oss << "Could not send ping, error: " << err << " " << err.message() << " client id=" << _id;
        throw ConnException(oss.str(), _id);
    }
    startPingTimer();
}
