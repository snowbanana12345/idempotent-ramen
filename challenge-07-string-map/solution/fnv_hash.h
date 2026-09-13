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

struct StringKey {
    const char* data;
    uint32_t    len;

    StringKey(const char* d, uint32_t l) : data(d), len(l) {}
    explicit StringKey(const char* d) : data(d), len(static_cast<uint32_t>(std::strlen(d))) {}
};

struct StringKeyHash {
    size_t operator()(const StringKey& k) const noexcept {
        return static_cast<size_t>(hash_key(k.data, k.len));
    }
};

struct StringKeyEq {
    bool operator()(const StringKey& a, const StringKey& b) const noexcept {
        return a.len == b.len && std::memcmp(a.data, b.data, a.len) == 0;
    }
};


class StringMap {
public:
    StringMap() = default;
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
    std::unordered_map<StringKey, uint32_t, StringKeyHash, StringKeyEq> map_;
};
}
