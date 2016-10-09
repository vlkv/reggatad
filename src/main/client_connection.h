class Service;
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/log/trivial.hpp>
#include <boost/function.hpp>
#include <iostream>
#include "service_exceptions.h"

#ifndef CLIENTCONNECTION_H_
#define CLIENTCONNECTION_H_

class ClientConnection {
	int _id;
	static int _next_id;

	boost::weak_ptr<Service> _service;
	boost::asio::ip::tcp::socket _sock;
	boost::asio::deadline_timer _pingTimer;

	enum { max_msg = 1024 };
	char _read_buffer[max_msg];
	char _write_buffer[max_msg];

public:
	typedef boost::shared_ptr<ClientConnection> ptr;

	ClientConnection(boost::asio::io_service& io_service, boost::shared_ptr<Service> service);
	virtual ~ClientConnection() = default;

	void start();
	void stop();

	boost::asio::ip::tcp::socket& sock();

private:
	void doRead();
	size_t isReadComplete(const boost::system::error_code& err, size_t bytes);
	void onRead(const boost::system::error_code& err, size_t bytes);
	void handleMsg(const std::string &msg);

	typedef boost::function<void(const boost::system::error_code &, size_t)> OnWriteHandler;
	void doWrite(const std::string &msg, OnWriteHandler onWriteHandler);

	void startPingTimer();
	void onPingTimer(const boost::system::error_code& err);
	void onPingSent(const boost::system::error_code& err, size_t bytes);
};

#endif /* CLIENTCONNECTION_H_ */
