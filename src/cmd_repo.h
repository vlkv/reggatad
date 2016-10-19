#pragma once
#include "cmd.h"
#include "repo.h"

struct CmdRepo : public Cmd {
	Repo* _repo;
	CmdRepo(const std::string& id, Cmd::SendResult sendResult);
	virtual ~CmdRepo() = default;

	void setContext(Repo* repo);

	virtual std::string path() const = 0;
};
