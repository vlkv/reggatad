/*
 * Service.cpp
 *
 *  Created on: Sep 28, 2016
 *      Author: vitvlkv
 */

#include "service.h"


Service::Service(int port, std::unique_ptr<Processor> proc) :
	_proc(std::move(proc)),
	_acceptor(_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
	_status(Service::Status::stopped) {
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
		stop();
	}
	catch (...) {
		std::cout << "Unexpected unknown exception";
		stop();
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
	auto cc_ptr = shared_from_this();
	std::cout << "Created shared from this...";
	_clients.push_back(std::unique_ptr<ClientConnection>(new ClientConnection(_service, cc_ptr)));
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

void Service::stop_async() {
	_service.dispatch(boost::bind(&Service::stop, shared_from_this()));
}

void Service::stop() {
	if (_status != Service::Status::started) {
		return;
	}
	std::cout << "Stopping server...";
	_status = Service::Status::stopping;

	_acceptor.close();
	std::cout << "tcp acceptor closed";

	// TODO: wait for clients to stop?..

	_clients.clear();
	_service.stop();
	_status = Service::Status::stopped;
}
