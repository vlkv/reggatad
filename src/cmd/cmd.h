#pragma once
#include <nlohmann/json.hpp>
namespace json = nlohmann;
#include <boost/assert.hpp>

#include <string>

struct Cmd {
    std::string _id;

    typedef std::function<void(const std::string&) > SendResult;
    SendResult _sendResult;

    Cmd(const std::string& id, SendResult sendResult);
    virtual ~Cmd() = default;

    static std::unique_ptr<Cmd> fromJson(const json::json& j, SendResult sendResult);

    virtual json::json execute() = 0;

    void sendResult(json::json& result);

private:
    template<class T>
    static std::unique_ptr<T> fromJson2(const json::json& j, SendResult sendResult) {
        std::string cmd = j["cmd"];
        BOOST_ASSERT_MSG(cmd == T::_name, "Bad cmd");

        const std::string id = j.at("id");
        auto res = std::unique_ptr<T>(new T(id, sendResult));

        auto args = j.value("args", json::json::object());
        for (auto& jm : T::_parseMap) {
            jm.second(args, *res);
        }
        return res;
    }
};
