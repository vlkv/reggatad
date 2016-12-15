#include "common.h"

namespace nlohmann {
    void PrintTo(const json& obj, ::std::ostream* os) {
        *os << obj.dump();
    }
}