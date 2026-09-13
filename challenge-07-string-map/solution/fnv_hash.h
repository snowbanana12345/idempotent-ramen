#pragma once
#include "base.h"

#include <unordered_map>

namespace hftu {

int hash_key(const char* key, size_t key_len) {
    uint64_t h = 1469598103934665603ull;       
    for (size_t i = 0; i < key_len; ++i) {
        h ^= static_cast<uint8_t>(key[i]);
        h *= 1099511628211ull;                  
    }
    return static_cast<int>(h);
}

class StringMap {
public:
    StringMap() = default;
    ~StringMap() = default;

    void insert(const char* key, size_t key_len, uint32_t value){
        map_.emplace(hash_key(key, key_len), value);
    }

    const uint32_t* find(const char* key, size_t key_len) const{
        auto it = map_.find(hash_key(key, key_len));
        if (it == map_.end()) return nullptr;
        return &it->second;
    }
    
private:
    std::unordered_map<int, uint32_t> map_;
};
}
