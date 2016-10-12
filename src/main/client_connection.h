#pragma once

#include "reggata_exceptions.h"
class Service;
#include "processor.h"

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/log/trivial.hpp>
#include <boost/function.hpp>

#include <iostream>
#include <memory>
#include <string>


class ClientConnection {
	int _id;
	static int _next_id;

	boost::asio::ip::tcp::socket _sock;
	boost::asio::deadline_timer _pingTimer;

	const size_t HEADER_SIZE = 4;

	enum { max_msg = 1024 };
	std::vector<char> _read_buffer;
	char _write_buffer[max_msg];

	std::shared_ptr<Processor> _proc;

public:
	typedef boost::shared_ptr<ClientConnection> ptr;

	ClientConnection(boost::asio::io_service& io_service, std::shared_ptr<Processor> proc);
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

	typedef boost::function<void(const boost::system::error_code &, size_t)> OnWriteHandler;
	void doWrite(const std::string &msg, OnWriteHandler onWriteHandler);

	void startPingTimer();
	void onPingTimer(const boost::system::error_code& err);
	void onPingSent(const boost::system::error_code& err, size_t bytes);
};
