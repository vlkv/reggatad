#pragma once
#include "cmd.h"
#include <memory>
class Repo;

struct CmdRepo : public Cmd {
	std::shared_ptr<Repo> _repo;
	CmdRepo(const std::string& id, Cmd::SendResult sendResult);
	virtual ~CmdRepo() = default;

	void setContext(std::shared_ptr<Repo> repo);

	virtual std::string path() const = 0;
};
