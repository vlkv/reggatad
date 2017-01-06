#include "service.h"
#include <boost/format.hpp>

Service::Service(int port, std::shared_ptr<Processor> proc, bool pingClients) :
_proc(proc),
_acceptor(_ioService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
_status(Service::Status::stopped),
_pingClients(pingClients) {
}

void Service::startListenPort() {
    BOOST_LOG_TRIVIAL(info) << "Service start";
    _status = Service::Status::started;
    try {
        acceptClient();
        serviceRunLoop();
    } catch (const ReggataException& e) {
        BOOST_LOG_TRIVIAL(error) << "Unexpected ReggataException: " << e.what();
        stop();
    } catch (const std::exception& e) {
        BOOST_LOG_TRIVIAL(error) << "Unexpected std::exception: " << e.what();
        stop();
    } catch (...) {
        BOOST_LOG_TRIVIAL(error) << "Unexpected unknown exception";
        stop();
    }
}

void Service::serviceRunLoop() {
    while (_status != Service::Status::stopped) {
        try {
            _ioService.run();
        } catch (const ConnException &e) {
            BOOST_LOG_TRIVIAL(error) << "Service fail: " << e.what();
            auto c = _clients.find(e.clientId());
            if (c != _clients.end()) {
                c->second->stop();
                _clients.erase(e.clientId());
            }
        } catch (...) {
            throw;
        }
    }
}

void Service::acceptClient() {
    if (!_acceptor.is_open()) {
        return;
    }
    BOOST_LOG_TRIVIAL(info) << "Waiting for client...";
    auto conn = std::unique_ptr<ClientConnection>(new ClientConnection(_ioService, _proc, _pingClients));
    auto connId = conn->id();
    _clients.insert(ClientConnections::value_type(connId, std::move(conn)));
    auto client = _clients.at(connId).get();
    _acceptor.async_accept(client->sock(), boost::bind(&Service::onAccept, this, client, _1));
}

void Service::onAccept(ClientConnection* client, const boost::system::error_code& err) {
    if (err == boost::asio::error::operation_aborted) {
        throw ReggataException("Accept operation aborted");
    }
    if (err) {
        throw ReggataException((boost::format("onAccept error: %1%") % err).str());
    }
    BOOST_LOG_TRIVIAL(info) << "Client id=" << client->id() << " accepted";
    client->start();
    acceptClient();
}

void Service::stopAsync() {
    _ioService.dispatch(boost::bind(&Service::stop, this));
}

bool Service::isStopped() {
    return _status == Service::Status::stopped;
}

void Service::stop() {
    if (_status != Service::Status::started) {
        return;
    }
    BOOST_LOG_TRIVIAL(info) << "Stopping Service...";
    _status = Service::Status::stopping;

    _acceptor.close();
    BOOST_LOG_TRIVIAL(info) << "tcp acceptor closed";

    for (auto c = _clients.begin(); c != _clients.end(); ++c) {
        c->second->stop();
    }

    _clients.clear();
    _ioService.stop();
    _status = Service::Status::stopped;

    BOOST_LOG_TRIVIAL(info) << "Service stopped";
}
