#pragma once
#include <json.hpp>

namespace nlohmann {
	// NOTE: this fixes segmentation fault
    void PrintTo(const json& obj, ::std::ostream* os) {
        *os << obj.dump();
    }
}