#include "client_connection.h"

ClientConnection::ClientConnection(boost::asio::io_service& io_service, boost::shared_ptr<Service> service) :
	_sock(io_service),
	_service(service) {
}

void ClientConnection::start() {

}

void ClientConnection::stop() {

}

boost::asio::ip::tcp::socket& ClientConnection::sock() {
	return _sock;
}
