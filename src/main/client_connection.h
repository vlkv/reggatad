class Service;
#include <boost/asio.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>

#ifndef CLIENTCONNECTION_H_
#define CLIENTCONNECTION_H_

class ClientConnection {
	boost::asio::ip::tcp::socket _sock;
	boost::weak_ptr<Service> _service;

public:
	typedef boost::shared_ptr<ClientConnection> ptr;

	ClientConnection(boost::asio::io_service& io_service, boost::shared_ptr<Service> service);
	virtual ~ClientConnection();

	void start();
	void stop();

	boost::asio::ip::tcp::socket& sock();
};

#endif /* CLIENTCONNECTION_H_ */
