#pragma once
#include <nlohmann/json.hpp>

namespace nlohmann {
    // NOTE: this fixes segmentation fault
    void PrintTo(const json& obj, ::std::ostream* os);
}