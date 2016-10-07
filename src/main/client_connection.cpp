#include "client_connection.h"

int ClientConnection::_next_id = 1;

ClientConnection::ClientConnection(boost::asio::io_service& io_service, boost::shared_ptr<Service> service) :
		_id(ClientConnection::_next_id++),
		_sock(io_service),
		_service(service) {
}

void ClientConnection::start() {
	do_read();
}

void ClientConnection::stop() {

}

boost::asio::ip::tcp::socket& ClientConnection::sock() {
	return _sock;
}

void ClientConnection::do_read() {
	boost::asio::async_read(_sock, boost::asio::buffer(_read_buffer),
		boost::bind(&ClientConnection::is_read_complete, this, _1, _2),
		boost::bind(&ClientConnection::on_read, this, _1, _2));
}

size_t ClientConnection::is_read_complete(const boost::system::error_code &err, size_t bytes) {
	if (err) {
		std::cout << "read_complete error: " << err << " client id=" << _id;
		return 0;
	}
	bool found = std::find(_read_buffer, _read_buffer + bytes, '\n') < _read_buffer + bytes;
	return found ? 0 : 1;
}

void ClientConnection::on_read(const boost::system::error_code &err, size_t bytes) {
	if (err) {
		std::cout << "on_read error: " << err << " client id=" << _id;
		throw ServiceException("on_read error");
	}
	std::string msg(_read_buffer, bytes);
	std::cout << "Received from client id=" << _id << " msg: " << msg;
	handle_msg(msg);
}

void ClientConnection::handle_msg(const std::string &msg) {
	std::cout << "TODO: process message " << msg;

}

