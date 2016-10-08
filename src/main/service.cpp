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
	BOOST_LOG_TRIVIAL(info) << "Service start";
	_status = Service::Status::started;
	try {
		acceptClient();
		serviceRunLoop();
	}
	catch (const std::exception &e) {
		BOOST_LOG_TRIVIAL(error) << "Unexpected std::exception: " << e.what();
		stop();
	}
	catch (...) {
		BOOST_LOG_TRIVIAL(error) << "Unexpected unknown exception";
		stop();
	}
}

void Service::serviceRunLoop() {
	while (_status != Service::Status::stopped) {
		try {
			_service.run();
		}
		catch (const ServiceException &e) {
			BOOST_LOG_TRIVIAL(error) << "Service fail: " << e.what();
			/*e.client()->stop();
			_clients.remove(e.client());*/
		}
		catch (...) {
			throw;
		}
	}
}

void Service::acceptClient() {
	if (!_acceptor.is_open()) {
		return;
	}
	BOOST_LOG_TRIVIAL(info) << "Waiting for client...";
	_clients.push_back(std::unique_ptr<ClientConnection>(new ClientConnection(_service, shared_from_this())));
	auto client = _clients.back().get();
	_acceptor.async_accept(client->sock(), boost::bind(&Service::onAccept, shared_from_this(), client, _1));
}

void Service::onAccept(ClientConnection* client, const boost::system::error_code& err) {
	if (err.value() == boost::asio::error::operation_aborted) {
		throw ServiceException("Accept operation aborted");
	}
	if (err) {
		std::ostringstream oss;
		oss << "on_accept error: " << err;
		throw ServiceException(oss.str());
	}
	BOOST_LOG_TRIVIAL(info) << "Client accepted!";
	client->start();
	acceptClient();
}

void Service::stopAsync() {
	_service.dispatch(boost::bind(&Service::stop, shared_from_this()));
}

void Service::stop() {
	if (_status != Service::Status::started) {
		return;
	}
	BOOST_LOG_TRIVIAL(info) << "Stopping server...";
	_status = Service::Status::stopping;

	_acceptor.close();
	BOOST_LOG_TRIVIAL(info) << "tcp acceptor closed";

	// TODO: wait for clients to stop?..

	_clients.clear();
	_service.stop();
	_status = Service::Status::stopped;
}
