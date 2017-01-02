#pragma once
#include <nlohmann/json.hpp>
namespace json = nlohmann;
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <list>

namespace JsonMap {

template<class C, class F>
void parseValue(const json::json& j, C& obj, const std::string& key, F C::*f) {
    try {
        F fieldVal = j.at(key);
        obj.*f = fieldVal;
    } catch(const std::out_of_range&) {
        // do nothing
    }
}

template<class C, class F>
void parseArray(const json::json& j, C& obj, const std::string& key, F C::*f) {
    try {
        for (auto& item : j.at(key)) {
            (obj.*f).push_back(item);
        }
    } catch(const std::out_of_range&) {
        // do nothing
    }
}

template<class C, class F>
std::pair<std::string, boost::function<void(const json::json&, C& obj)>> 
mapValue(const std::string& key, F C::*fieldPtr) {
    auto fun = boost::bind(&parseValue<C, F>, _1, _2, key, fieldPtr);
    return std::pair<std::string, boost::function<void(const json::json&, C&)>>(key, fun);
}

template<class C, class F>
std::pair<std::string, boost::function<void(const json::json&, C& obj)>> 
mapArray(const std::string& key, F C::*fieldPtr) {
    auto fun = boost::bind(&parseArray<C, F>, _1, _2, key, fieldPtr);
    return std::pair<std::string, boost::function<void(const json::json&, C&)>>(key, fun);
}

template<class C>
using ParseMap = std::list<std::pair<std::string, boost::function<void(const json::json&, C& obj)>>>;

}