#include "ClientConnection.h"

ClientConnection::ClientConnection(boost::asio::io_service& service, boost::weak_ptr<Service> server) :
	_sock(service) {
}

ClientConnection::~ClientConnection() {
}

void ClientConnection::start() {

}

void ClientConnection::stop() {

}

boost::asio::ip::tcp::socket& ClientConnection::sock() {
	return _sock;
}
