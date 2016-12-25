#pragma once
#include <rocksdb/db.h>
#include <rocksdb/merge_operator.h>

class CompareAndIncUInt64 : public rocksdb::AssociativeMergeOperator {
public:
    virtual const char* Name() const {
        return "reggata.CompareAndIncUInt64";
    }
    
    /* To use this MergeOperator client should supply the expected current value.
     * If the expected value is equal to actual value, then MergeOperator would
     * increment the value. So client could use (expected value + 1). Otherwise
     * MergeOperator would fail and does not change the actual value.
     */
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
                Log(logger, "existing value corruption");
                return false;
            }
        }
        
        uint64_t oper = std::strtoul(value.data(), nullptr, 10);
        if (oper == std::numeric_limits<uint64_t>::max()) {
            Log(logger, "operand value corruption");
            return false;
        }
        
        if (existing != oper) {
            Log(logger, "operand value is not equal to existing_value");
            return false;
        }

        auto new_ = existing + 1;
        *new_value = std::to_string(new_);
        return true;
    }

    
};
