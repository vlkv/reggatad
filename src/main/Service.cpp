/*
 * Service.cpp
 *
 *  Created on: Sep 28, 2016
 *      Author: vitvlkv
 */

#include "Service.h"
#include "service_exceptions.h"
#include <boost/bind.hpp>

Service::Service(int port) :
	_acceptor(_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
	_status(Service::Status::stopped) {
}

Service::~Service() {
}


void Service::openRepo(const std::string& repoRootPath, const std::string& repoDbPath) {
	_repos.push_back(std::unique_ptr<Repo>(new Repo(repoRootPath, repoDbPath)));
}

void Service::start() {
	std::cout << "Service start";
	_status = Service::Status::started;
	try {
		accept_client();
		service_run_loop();
	}
	catch (const std::exception &e) {
		std::cout << "Unexpected std::exception: " << e.what();
//		stop_finish();
	}
	catch (...) {
		std::cout << "Unexpected unknown exception";
//		stop_finish();
	}
}

void Service::service_run_loop() {
	while (_status != Service::Status::stopped) {
		try {
			_service.run();
		}
		catch (const ServiceException &e) {
			std::cout << "Service fail: " << e.what();
			/*e.client()->stop();
			_clients.remove(e.client());*/
		}
		catch (...) {
			throw;
		}
	}
}

void Service::accept_client() {
	if (!_acceptor.is_open()) {
		return;
	}
	std::cout << "Waiting for client...";
	_clients.push_back(std::unique_ptr<ClientConnection>(new ClientConnection(_service, weak_from_this())));
	auto client = _clients.back().get();
	_acceptor.async_accept(client->sock(), boost::bind(&Service::on_accept, shared_from_this(), client, _1));
}

void Service::on_accept(ClientConnection* client, const boost::system::error_code& err) {
	if (err.value() == boost::asio::error::operation_aborted) {
		throw ServiceException("Accept operation aborted");
	}
	if (err) {
		std::ostringstream oss;
		oss << "on_accept error: " << err;
		throw ServiceException(oss.str());
	}
	std::cout << "Client accepted!";
	client->start();
	accept_client();
}
