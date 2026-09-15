#pragma once
#include "base.h"

#include <unordered_map>
#include <iostream>

#include "fnv_hash.h"

namespace hftu {

class StringMap {
public:
    StringMap(){
        map_.reserve(1024 * 1024);
    }
    ~StringMap() = default;

    void insert(const char* key, size_t key_len, uint32_t value){
        map_.emplace(StringKey(key, key_len), value);
    }

    const uint32_t* find(const char* key, size_t key_len) const{
        auto it = map_.find(StringKey(key, key_len));
        if (it == map_.end()) return nullptr;
        return &it->second;
    }
    
private:
    std::unordered_map<StringKey, uint32_t, StringKeyHash<hash_key>, StringKeyEq> map_;
};
}
