#pragma once
#include "json.hpp"
namespace json = nlohmann;
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <list>

namespace JsonMap {

template<class C, class F>
void apply(const json::json& j, C& obj, const std::string& key, F C::*f) {
    try {
        F fieldVal = j.at(key);
        obj.*f = fieldVal;
    } catch(const std::out_of_range&) {
        // do nothing
    }
}

template<class C, class F>
std::pair<std::string, boost::function<void(const json::json&, C& obj)>> 
create(const std::string& key, F C::*fieldPtr) {
    auto fun = boost::bind(&apply<C, F>, _1, _2, key, fieldPtr);
    return std::pair<std::string, boost::function<void(const json::json&, C&)>>(key, fun);
}


template<class C>
using ParseMap = std::list<std::pair<std::string, boost::function<void(const json::json&, C& obj)>>>;

}