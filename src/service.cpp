/*
 * Service.cpp
 *
 *  Created on: Sep 28, 2016
 *      Author: vitvlkv
 */

#include "service.h"


Service::Service(int port, std::shared_ptr<Processor> proc) :
	_proc(proc),
	_acceptor(_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
	_status(Service::Status::stopped) {
}


void Service::startListenPort() {
	BOOST_LOG_TRIVIAL(info) << "Service start";
	_status = Service::Status::started;
	try {
		acceptClient();
		serviceRunLoop();
	}
	catch (const ReggataException& e) {
		BOOST_LOG_TRIVIAL(error) << "Unexpected ReggataException: " << e.what();
		stop();
	}
	catch (const std::exception& e) {
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
		catch (const ConnException &e) {
			BOOST_LOG_TRIVIAL(error) << "Service fail: " << e.what();
			_clients.at(e.clientId())->stop();
			_clients.erase(e.clientId());
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
	auto conn = std::unique_ptr<ClientConnection>(new ClientConnection(_service, _proc));
	auto connId = conn->id();
	_clients.insert(ClientConnections::value_type(connId, std::move(conn)));
	auto client = _clients.at(connId).get();
	_acceptor.async_accept(client->sock(), boost::bind(&Service::onAccept, this, client, _1));
}

void Service::onAccept(ClientConnection* client, const boost::system::error_code& err) {
	if (err.value() == boost::asio::error::operation_aborted) {
		throw ReggataException("Accept operation aborted");
	}
	if (err) {
		std::ostringstream oss;
		oss << "on_accept error: " << err;
		throw ReggataException(oss.str());
	}
	BOOST_LOG_TRIVIAL(info) << "Client accepted!";
	client->start();
	acceptClient();
}

void Service::stopAsync() {
	_service.dispatch(boost::bind(&Service::stop, this));
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
