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
