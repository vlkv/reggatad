#include "client.h"
#include "status_code.h"
#include "reggata_exceptions.h"
#include <gtest/gtest.h>
#include <boost/format.hpp>

int Client::_nextCmdId = 1;

Client::Client(int port) :
_socket(_ioService) {
    boost::asio::ip::tcp::endpoint endpoint(
            boost::asio::ip::address::from_string("127.0.0.1"), port);
    _socket.connect(endpoint);
}

void Client::send(const std::string& message) {
    std::cout << "Client => " << message << std::endl;
    std::vector<char> buf;
    buf.resize(4 + message.size());
    uint32_t header = message.size();
    std::copy((char*) &header, (char*) &header + 4, buf.begin());
    std::copy(message.begin(), message.end(), buf.begin() + 4);
    boost::system::error_code error;
    _socket.write_some(boost::asio::buffer(buf), error);
    if (error) {
        throw ReggataException((boost::format("ERROR: Client failed writing to socket: %1%") % error).str());
    }
}

std::string Client::recv() { // TODO: add timeout param
    std::vector<char> buf;
    buf.resize(4);
    boost::system::error_code error;
    _socket.read_some(boost::asio::buffer(buf), error);
    if (error) {
        throw ReggataException((boost::format("ERROR: Client failed reading message header: %1%") % error).str());
    }
    uint32_t header;
    std::copy(buf.begin(), buf.end(), (char*) &header);

    buf.resize(header);
    _socket.read_some(boost::asio::buffer(buf), error);
    if (error) {
        throw ReggataException((boost::format("ERROR: Client failed reading message body: %1%") % error).str());
    }
    auto message = std::string(buf.begin(), buf.end());
    std::cout << "Client <= " << message << std::endl;
    return message;
}

std::string Client::nextCmdId() {
    return std::to_string(_nextCmdId++);
}

nlohmann::json Client::initRepo(const boost::filesystem::path& rootDir, const boost::filesystem::path& dbDir) {
    return openRepo(rootDir, dbDir, true);
}

nlohmann::json Client::openRepo(const boost::filesystem::path& rootDir, const boost::filesystem::path& dbDir, bool initIfNotExists = false) {
    auto cmdId = nextCmdId();
    nlohmann::json cmd = {
        {"id", cmdId},
        {"cmd", "open_repo"},
        {"args",
            {
                {"root_dir", rootDir.c_str()},
                {"db_dir", dbDir.c_str()},
                {"init_if_not_exists", initIfNotExists}
            }}
    };
    send(cmd.dump());
    auto msg = recv();
    auto obj = nlohmann::json::parse(msg);
    if (cmdId != obj["id"]) {
        throw ReggataException((boost::format("Command id %1% != response id %2%") % cmdId % obj["id"]).str());
    }
    return obj;
}

nlohmann::json Client::addTags(const boost::filesystem::path& absFile, const std::vector<std::string>& tags) {
    auto cmdId = nextCmdId();
    nlohmann::json cmd = {
        {"id", cmdId},
        {"cmd", "add_tags"},
        {"args",
            {
                {"file", absFile.c_str()},
                {"tags", tags}
            }}
    };
    send(cmd.dump());
    auto msg = recv();
    auto obj = nlohmann::json::parse(msg);
    if (cmdId != obj["id"]) {
        throw ReggataException((boost::format("Command id %1% != response id %2%") % cmdId % obj["id"]).str());
    }
    return obj;
}

nlohmann::json Client::removeTags(const boost::filesystem::path& absFile, const std::vector<std::string>& tags) {
    auto cmdId = nextCmdId();
    nlohmann::json cmd = {
        {"id", cmdId},
        {"cmd", "remove_tags"},
        {"args",
            {
                {"file", absFile.c_str()},
                {"tags", tags}
            }}
    };
    send(cmd.dump());
    auto msg = recv();
    auto obj = nlohmann::json::parse(msg);
    if (cmdId != obj["id"]) {
        throw ReggataException((boost::format("Command id %1% != response id %2%") % cmdId % obj["id"]).str());
    }
    return obj;
}

nlohmann::json Client::getFileInfo(const boost::filesystem::path & absFile) {
    auto cmdId = nextCmdId();
    nlohmann::json cmd = {
        {"id", cmdId},
        {"cmd", "get_file_info"},
        {"args",
            {
                {"file", absFile.c_str()},
            }}
    };
    send(cmd.dump());
    auto msg = recv();
    auto obj = nlohmann::json::parse(msg);
    if (cmdId != obj["id"]) {
        throw ReggataException((boost::format("Command id %1% != response id %2%") % cmdId % obj["id"]).str());
    }
    return obj;
}

nlohmann::json Client::search(const boost::filesystem::path& absFile, const std::string& query) {
    auto cmdId = nextCmdId();
    nlohmann::json cmd = {
        {"id", cmdId},
        {"cmd", "search"},
        {"args",
            {
                {"dir", absFile.c_str()},
                {"query", query},
            }}
    };
    send(cmd.dump());
    auto msg = recv();
    auto obj = nlohmann::json::parse(msg);
    if (cmdId != obj["id"]) {
        throw ReggataException((boost::format("Command id %1% != response id %2%") % cmdId % obj["id"]).str());
    }
    return obj;
}
