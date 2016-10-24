#pragma once
#include "cmd.h"
class Repo;

struct CmdRepo : public Cmd {
	Repo* _repo; // TODO: use shared_ptr
	CmdRepo(const std::string& id, Cmd::SendResult sendResult);
	virtual ~CmdRepo() = default;

	void setContext(Repo* repo);

	virtual std::string path() const = 0;
};
