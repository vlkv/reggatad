#pragma once
#include "repo.h"
#include "client_connection.h"
#include "processor.h"
#include "reggata_exceptions.h"

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>

#include <map>
#include <vector>
#include <list>
#include <memory>
#include <string>
#include <iostream>

class Service {
    std::shared_ptr<Processor> _proc;

    boost::asio::io_service _ioService;
    boost::asio::ip::tcp::acceptor _acceptor;

    enum Status {
        starting, started, stopping, stopped
    };
    Status _status;

    typedef std::map<int, std::unique_ptr<ClientConnection>> ClientConnections;
    ClientConnections _clients;

    bool _pingClients;

public:
    Service(int port, std::shared_ptr<Processor> proc, bool pingClients);
    virtual ~Service() = default;

    void startListenPort();
    void stopAsync();

private:
    void serviceRunLoop();
    void acceptClient();
    void onAccept(ClientConnection* client, const boost::system::error_code& err);
    void stop();
};
