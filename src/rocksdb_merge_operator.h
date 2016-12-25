#pragma once
#include <rocksdb/db.h>
#include <rocksdb/merge_operator.h>

class UInt64AddOperator : public rocksdb::AssociativeMergeOperator {
public:

    // TODO: reimplement this class to be a Compare-And-Set operation
    
    virtual bool Merge(
            const rocksdb::Slice& key,
            const rocksdb::Slice* existing_value,
            const rocksdb::Slice& value,
            std::string* new_value,
            rocksdb::Logger* logger) const {

        // assuming 0 if no existing value
        uint64_t existing = 0;
        if (existing_value) {
            existing = std::strtoul(existing_value->data(), nullptr, 10);
            if (existing == std::numeric_limits<uint64_t>::max()) {
                // if existing_value is corrupted, treat it as 0
                Log(logger, "existing value corruption");
                existing = 0;
            }
        }
        
        uint64_t oper = std::strtoul(value.data(), nullptr, 10);
        if (oper == std::numeric_limits<uint64_t>::max()) {
            // if operand is corrupted, treat it as 0
            Log(logger, "operand value corruption");
            oper = 0;
        }

        auto new_ = existing + oper;
        *new_value = std::to_string(new_);
        return true; // always return true for this, since we treat all errors as "zero".
    }

    virtual const char* Name() const {
        return "UInt64AddOperator";
    }
};


class Counters {
 public:
  // (re)set the value of a named counter
  //virtual void Set(const string& key, uint64_t value);

  // remove the named counter
  //virtual void Remove(const string& key);

  // retrieve the current value of the named counter, return false if not found
  //virtual bool Get(const string& key, uint64_t *value);

  // increase the named counter by value.
  // if the counter does not exist,  treat it as if the counter was initialized to zero
  virtual void Add(const std::string& key, uint64_t value);
  };


class MergeBasedCounters : public Counters {
    std::shared_ptr<rocksdb::DB> _db;
public:
    MergeBasedCounters(std::shared_ptr<rocksdb::DB> db) :
    _db(db) {};

    virtual void Add(const std::string& key, uint64_t value) {
        std::string serialized = std::to_string(value);
        rocksdb::WriteOptions wo;
        wo.sync = true;
        _db->Merge(wo, key, serialized);
    }
};

/*
// How to use it
DB* dbp;
Options options;
options.merge_operator.reset(new UInt64AddOperator);
DB::Open(options, "/tmp/db", &dbp);
std::shared_ptr<DB> db(dbp);
MergeBasedCounters counters(db);
counters.Add("a", 1);
...
uint64_t v;
counters.Get("a", &v);
*/