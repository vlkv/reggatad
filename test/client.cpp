#include "client.h"

Client::Client(int port) :
	_socket(_ios) {
	boost::asio::ip::tcp::endpoint endpoint(
			boost::asio::ip::address::from_string("127.0.0.1"), port);
	_socket.connect(endpoint);
}

boost::system::error_code Client::send(const std::string& message) {
	std::cout << "Client send: " << message << std::endl;
	std::vector<char> buf;
	buf.resize(4 + message.size());
	uint32_t header = message.size();
	std::copy((char*)&header, (char*)&header+4, buf.begin());
	std::copy(message.begin(), message.end(), buf.begin()+4);
	boost::system::error_code error;
	_socket.write_some(boost::asio::buffer(buf), error);
	return error;
}

std::string Client::recv() {
	std::vector<char> buf;
	buf.resize(4);
	boost::system::error_code error;
	_socket.read_some(boost::asio::buffer(buf), error);
	if (error) {
		std::cout << "error reading header: " << error << std::endl;
	}
	uint32_t header;
	std::copy(buf.begin(), buf.end(), (char*)&header);

	buf.resize(header);
	_socket.read_some(boost::asio::buffer(buf), error);
	if (error) {
		std::cout << "error reading message: " << error << std::endl;
	}
	auto message = std::string(buf.begin(), buf.end());
	std::cout << "Client recv: " << message << std::endl;
	return message;
}
