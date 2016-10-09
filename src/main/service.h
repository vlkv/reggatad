#pragma once
#include "repo.h"
#include "client_connection.h"
#include "processor.h"
#include "reggata_exceptions.h"

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/log/trivial.hpp>

#include <map>
#include <vector>
#include <list>
#include <memory>
#include <string>
#include <iostream>


class Service : public boost::enable_shared_from_this<Service> {
	std::unique_ptr<Processor> _proc;

	boost::asio::io_service _service; // TODO: rename to _io_service
	boost::asio::ip::tcp::acceptor _acceptor;

	enum Status { starting, started, stopping, stopped };
	Status _status;

	typedef std::map<int, std::unique_ptr<ClientConnection>> ClientConnections;
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
