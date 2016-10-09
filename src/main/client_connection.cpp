#include "client_connection.h"

int ClientConnection::_next_id = 1;

ClientConnection::ClientConnection(boost::asio::io_service& io_service, boost::weak_ptr<Service> service) :
		_id(ClientConnection::_next_id++),
		_service(service),
		_sock(io_service),
		_pingTimer(io_service) {
}

void ClientConnection::start() {
	startPingTimer();
	doRead();
}

void ClientConnection::stop() {
}

void ClientConnection::onPingTimer(const boost::system::error_code& err) {
	BOOST_LOG_TRIVIAL(debug) << "onPingTimer, err=" << err;
	doWrite("{cmd:\"ping\"}\n", boost::bind(&ClientConnection::onPingSent, this, _1, _2));
}

void ClientConnection::startPingTimer() {
	_pingTimer.expires_from_now(boost::posix_time::seconds(5));
	_pingTimer.async_wait(boost::bind(&ClientConnection::onPingTimer, this, _1));
}

boost::asio::ip::tcp::socket& ClientConnection::sock() {
	return _sock;
}

int ClientConnection::id() const {
	return _id;
}

void ClientConnection::doRead() {
	BOOST_LOG_TRIVIAL(debug) << "doRead";
	boost::asio::async_read(_sock, boost::asio::buffer(_read_buffer),
		boost::bind(&ClientConnection::isReadComplete, this, _1, _2),
		boost::bind(&ClientConnection::onRead, this, _1, _2));
}

size_t ClientConnection::isReadComplete(const boost::system::error_code &err, size_t bytes) {
	BOOST_LOG_TRIVIAL(debug) << "isReadComplete err=" << err << " bytes=" << bytes;
	if (err) {
		BOOST_LOG_TRIVIAL(debug) << "read_complete error: " << err << " client id=" << _id;
		return 0;
	}
	bool found = std::find(_read_buffer, _read_buffer + bytes, '\n') < _read_buffer + bytes;
	return found ? 0 : 1;
}

void ClientConnection::onRead(const boost::system::error_code &err, size_t bytes) {
	BOOST_LOG_TRIVIAL(debug) << "onRead";
	if (err) {
		BOOST_LOG_TRIVIAL(error) << "onRead error: " << err << " client id=" << _id;
		throw ConnException("onRead error", weak_from_this());
	}
	std::string msg(_read_buffer, bytes);
	BOOST_LOG_TRIVIAL(info) << "Received from client id=" << _id << " msg: " << msg;
	handleMsg(msg);
}

void ClientConnection::handleMsg(const std::string &msg) {
	BOOST_LOG_TRIVIAL(debug) << "TODO: parse request and delegate it to Processor. Then take results, serialize them and send response." << msg;
}

void ClientConnection::doWrite(const std::string &msg, OnWriteHandler onWriteHandler) {
	BOOST_LOG_TRIVIAL(info) << "Sending to client id=" << _id << " msg: " << msg;
	std::copy(msg.begin(), msg.end(), _write_buffer);
	boost::asio::async_write(_sock, boost::asio::buffer(_write_buffer, msg.size()), onWriteHandler);
}

void ClientConnection::onPingSent(const boost::system::error_code& err, size_t bytes) {
	if (err) {
		std::ostringstream oss;
		oss << "Could not send ping, error: " << err << " client id=" << _id;
		throw ConnException(oss.str(), weak_from_this());
	}
	startPingTimer();
}
