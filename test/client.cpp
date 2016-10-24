#include "client.h"

Client::Client(int port) :
	_socket(_ios) {
	boost::asio::ip::tcp::endpoint endpoint(
			boost::asio::ip::address::from_string("127.0.0.1"), port);
	_socket.connect(endpoint);
}

boost::system::error_code Client::send(const std::string& message) {
	std::vector<char> buf;
	buf.resize(4 + message.size());
	uint32_t header = message.size();
	std::cout << "message size is " << header << std::endl;
	std::copy((char*)&header, (char*)&header+4, buf.begin());
	std::copy(message.begin(), message.end(), buf.begin()+4);
	boost::system::error_code error;
	_socket.write_some(boost::asio::buffer(buf), error);
	return error;
}

std::string Client::recv() {
	// TODO: read header with the msg length then read the message
	boost::array<char, 128> buf;
	boost::system::error_code error;
	_socket.read_some(boost::asio::buffer(buf), error);
	std::string message(buf.c_array());
	return message;
}
