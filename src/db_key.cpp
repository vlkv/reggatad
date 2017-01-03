#include "db_key.h"
#include <string>
#include <vector>

const char DBKey::DELIM = ':';
const char DBKey::ESC = '\\';

std::string DBKey::join(const std::string& prefix, const std::string& suffix) {
    return DBKey::escape(prefix) + DELIM + DBKey::escape(suffix);
}

std::pair<std::string, std::string> DBKey::split(const std::string& key) {
    auto i = key.begin();
    for (; i != key.end(); ++i) {
        if (*i == DELIM && (i == key.begin() || *(i - 1) != ESC)) {
            break;
        }
    }
    auto pos = i - key.begin();
    auto prefix = key.substr(0, pos);
    auto suffix = i != key.end() ? key.substr(pos + 1, key.end() - i) : std::string("");
    return std::pair<std::string, std::string>(prefix, suffix);
}

std::string DBKey::escape(const std::string& str) {
    std::vector<char> buf;
    for (auto i = str.begin(); i != str.end(); ++i) {
        if (*i == DELIM || *i == ESC) {
            buf.push_back(ESC);
        }
        buf.push_back(*i);
    }
    return std::string(buf.begin(), buf.end());
}

std::string DBKey::unescape(const std::string & str) {
    std::vector<char> buf;
    for (auto i = str.begin(); i != str.end(); ++i) {
        if (*i == ESC) {
            auto j = i + 1;
            if (j != str.end() && (*j == DELIM || *j == ESC)) {
                ++i;
            }
        }
        buf.push_back(*i);
    }
    return std::string(buf.begin(), buf.end());
}