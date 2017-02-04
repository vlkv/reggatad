#pragma once
#include <nlohmann/json.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <list>

namespace JsonMap {

template<class C, class F>
void parseValue(const nlohmann::json& j, C& obj, const std::string& key, F C::*f) {
    F fieldVal = j.at(key);
    obj.*f = fieldVal;
}

template<class C, class F>
void parseValue(const nlohmann::json& j, C& obj, const std::string& key, F C::*f, F defaultValue) {
    F fieldVal = j.value(key, defaultValue);
    obj.*f = fieldVal;
}

template<class C, class F>
void parseArray(const nlohmann::json& j, C& obj, const std::string& key, F C::*f) {
    for (auto& item : j.at(key)) {
        (obj.*f).push_back(item);
    }
}

template<class C, class F>
std::pair<std::string, boost::function<void(const nlohmann::json&, C& obj)>> 
mapValue(const std::string& key, F C::*fieldPtr) {
    auto fun = boost::bind(&parseValue<C, F>, _1, _2, key, fieldPtr);
    return std::pair<std::string, boost::function<void(const nlohmann::json&, C&)>>(key, fun);
}

template<class C, class F>
std::pair<std::string, boost::function<void(const nlohmann::json&, C& obj)>> 
mapValue(const std::string& key, F C::*fieldPtr, F defaultValue) {
    auto fun = boost::bind(&parseValue<C, F>, _1, _2, key, fieldPtr, defaultValue);
    return std::pair<std::string, boost::function<void(const nlohmann::json&, C&)>>(key, fun);
}

template<class C, class F>
std::pair<std::string, boost::function<void(const nlohmann::json&, C& obj)>> 
mapArray(const std::string& key, F C::*fieldPtr) {
    auto fun = boost::bind(&parseArray<C, F>, _1, _2, key, fieldPtr);
    return std::pair<std::string, boost::function<void(const nlohmann::json&, C&)>>(key, fun);
}

template<class C>
using ParseMap = std::list<std::pair<std::string, boost::function<void(const nlohmann::json&, C& obj)>>>;

}