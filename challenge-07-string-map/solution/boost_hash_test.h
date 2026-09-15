#pragma once
#include "base.h"

#include <unordered_map>
#include <boost/functional/hash.hpp>

namespace hftu {

std::size_t hash_key(const char* s, std::size_t key_len) {
    std::size_t seed = 0;
    boost::hash_range(seed, s, s + key_len);
    return seed;
}


class StringMap {
public:
    StringMap(){

    }
    ~StringMap() = default;

    void insert(const char* key, size_t key_len, uint32_t value){
        uint64_t hash = hash_key(key, key_len);
        hash_buffer_[key_len] = hash;
    }

    const uint32_t* find(const char* key, size_t key_len) const{
        uint64_t hash = hash_key(key, key_len);
        return &buffer[hash % 16];
    }
    
private:
    uint64_t hash_buffer_[17];
    uint32_t buffer[17];
};
}
