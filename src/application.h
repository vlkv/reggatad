#pragma once
#include "service.h"
#include <string>
#include <vector>
#include <memory>

class Processor;

class Application {
    std::shared_ptr<Processor> _proc;
    std::unique_ptr<Service> _service;

public:
    Application(int port, int pingClientsIntervalMs = 5000);
    virtual ~Application() = default;
    void openRepo(const std::string& rootPath, const std::string& dbPath);
    void start();
    void stop();
};
