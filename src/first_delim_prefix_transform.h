#pragma once
#include <rocksdb/db.h>
#include <rocksdb/slice_transform.h>
#include <iostream>

class FirstDelimPrefixTransform : public rocksdb::SliceTransform {
    std::string _delim; // TODO: add _delimEsc, and skip _delimEsc in keys
    std::string _name;

public:
    explicit FirstDelimPrefixTransform(const std::string& delim)
    : _delim(delim),
    // Note that if any part of the name format changes, it will require
    // changes on options_helper in order to make RocksDBOptionsParser work
    // for the new change.
    _name("reggata.FirstDelimPrefixTransform." + _delim) {
    }

    virtual const char* Name() const {
        return _name.c_str();
    }

    virtual rocksdb::Slice Transform(const rocksdb::Slice& src) const {
        assert(InDomain(src));
        // TODO: optimize, do not use std::string
        auto str = src.ToString();
        auto i = str.find_first_of(_delim);
        auto res = rocksdb::Slice(src.data(), i);
        return res;
    }

    virtual bool InDomain(const rocksdb::Slice& src) const {
        // TODO: optimize, do not use std::string
        auto str = src.ToString();
        return str.find_first_of(_delim) != str.size(); // TODO: Maybe we should return true for keys without a delimiter?..
    }

    virtual bool InRange(const rocksdb::Slice& dst) const {
        return true;
    }
};