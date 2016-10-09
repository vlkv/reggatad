#ifndef SERVICE_H_
#define SERVICE_H_

#include "repo.h"
#include "client_connection.h"
#include "processor.h"
#include "service_exceptions.h"

#include <vector>
#include <list>
#include <memory>
#include <string>
#include <iostream>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/log/trivial.hpp>



class Service : public boost::enable_shared_from_this<Service> {
	std::unique_ptr<Processor> _proc;

	boost::asio::io_service _service; // TODO: rename to _io_service
	boost::asio::ip::tcp::acceptor _acceptor;

	enum Status { starting, started, stopping, stopped };
	Status _status;

	typedef std::vector<std::unique_ptr<ClientConnection>> ClientConnections;
	ClientConnections _clients;

public:
	Service(int port, std::unique_ptr<Processor> proc);
	virtual ~Service() = default;

	void start();
	void stopAsync();

private:
	void serviceRunLoop();
	void acceptClient();
	void onAccept(ClientConnection* client, const boost::system::error_code& err);
	void stop();
};

#endif /* SERVICE_H_ */