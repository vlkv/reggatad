#pragma once
#include "cmd.h"
#include <memory>
class Processor;

struct CmdProc : public Cmd {
    std::shared_ptr<Processor> _proc;

    CmdProc(const std::string& id, Cmd::SendResult sendResult);
    virtual ~CmdProc() = default;

    void setContext(std::shared_ptr<Processor> proc);
};
