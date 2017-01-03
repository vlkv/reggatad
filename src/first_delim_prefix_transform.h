#pragma once
#include "db_key.h"
#include <rocksdb/db.h>
#include <rocksdb/slice_transform.h>
#include <iostream>


class FirstDelimPrefixTransform : public rocksdb::SliceTransform {
    std::string _name;

public:
    FirstDelimPrefixTransform()
    :
    // Note that if any part of the name format changes, it will require
    // changes on options_helper in order to make RocksDBOptionsParser work
    // for the new change.
    _name("reggata.FirstDelimPrefixTransform") {
    }

    virtual const char* Name() const {
        return _name.c_str();
    }

    virtual rocksdb::Slice Transform(const rocksdb::Slice& src) const {
        assert(InDomain(src));
        
        auto data = src.data();
        auto dataSize = src.size();
        size_t i = 0;
        for (; i < dataSize; ++i) {
            while (i < dataSize && data[i] != DBKey::DELIM) {
                ++i;
            }
            if (i == 0) {
                break;
            }
            if (i == dataSize) {
                break;
            }
            if (data[i-1] != DBKey::ESC) {
                break;
            }
        }
        auto res = rocksdb::Slice(src.data(), i);
        return res;
    }

    virtual bool InDomain(const rocksdb::Slice& src) const {
        return true;
    }

    virtual bool InRange(const rocksdb::Slice& dst) const {
        return true;
    }
};