class Service;
#include <boost/asio.hpp>
#include <boost/weak_ptr.hpp>

#ifndef CLIENTCONNECTION_H_
#define CLIENTCONNECTION_H_

class ClientConnection {
	boost::asio::ip::tcp::socket _sock;

public:
	typedef boost::shared_ptr<ClientConnection> ptr;

	ClientConnection(boost::asio::io_service& io_service, boost::weak_ptr<Service> server);
	virtual ~ClientConnection();

	void start();
	void stop();

	boost::asio::ip::tcp::socket& sock();
};

#endif /* CLIENTCONNECTION_H_ */
