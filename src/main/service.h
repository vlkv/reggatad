#ifndef SERVICE_H_
#define SERVICE_H_

#include "repo.h"
#include "client_connection.h"

#include <vector>
#include <list>
#include <memory>
#include <string>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <iostream>



class Service : public boost::enable_shared_from_this<Service> {
	std::vector<std::unique_ptr<Repo>> _repos;

	boost::asio::io_service _service; // TODO: rename to _io_service
	boost::asio::ip::tcp::acceptor _acceptor;

	enum Status { starting, started, stopping, stopped };
	Status _status;

	typedef std::list<std::unique_ptr<ClientConnection>> ListOfClients;
	ListOfClients _clients;

public:
	Service(int port);
	virtual ~Service();

	void openRepo(const std::string& repoRootPath, const std::string& repoDbPath);
	void start();
	void stop_async();

private:
	void service_run_loop();
	void accept_client();
	void on_accept(ClientConnection* client, const boost::system::error_code& err);
	void stop();
};

#endif /* SERVICE_H_ */
