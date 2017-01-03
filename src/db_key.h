#pragma once
#include <string>

class DBKey {
public:
    static const char DELIM;
    static const char ESC;
    
    static std::string join(const std::string& prefix, const std::string& suffix);
    static std::pair<std::string, std::string> split(const std::string& key);

private:
    static std::string escape(const std::string& str);
    static std::string unescape(const std::string& str);
};
