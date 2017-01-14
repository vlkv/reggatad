#include "tag.h"

Tag::Tag(const std::string& name) : _name(name) {
}

std::string Tag::str() {
    return _name;
}
