#pragma once
#include "processor.h"
#include "service.h"
#include <string>
#include <vector>

class Application {
	std::shared_ptr<Processor> _proc;
	std::unique_ptr<Service> _service;


public:
	Application(int port, bool pingClients);
	virtual ~Application() = default;
	void openRepo(const std::string& rootPath, const std::string& dbPath);
	void start();
	void stop();
};
