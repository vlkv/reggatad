#pragma once
#include <nlohmann/json.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/filesystem.hpp>
#include <memory>
#include <iostream>


class Client {
    static int _nextCmdId;
    
    boost::asio::io_service _ioService;
    boost::asio::ip::tcp::socket _socket;

public:
    Client(int port);
    virtual ~Client() = default;

    void send(const std::string& message);
    std::string recv();
    
    nlohmann::json initRepo(const boost::filesystem::path& rootDir, const boost::filesystem::path& dbDir);
    nlohmann::json openRepo(const boost::filesystem::path& rootDir, const boost::filesystem::path& dbDir, bool initIfNotExists);
    nlohmann::json getReposInfo();
    nlohmann::json addTags(const boost::filesystem::path& absFile, const std::vector<std::string>& tags);
    nlohmann::json removeTags(const boost::filesystem::path& absFile, const std::vector<std::string>& tags);
    nlohmann::json getFileInfo(const boost::filesystem::path& absFile);
    nlohmann::json search(const boost::filesystem::path& absFile, const std::string& query);
    
private:
    std::string nextCmdId();
};
