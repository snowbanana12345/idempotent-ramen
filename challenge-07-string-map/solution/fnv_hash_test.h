#include "base.h"

#include "fnv_hash.h"

namespace hftu {

size_t suffix_hash(const char* key, size_t key_len) {   
    uint64_t seg = key_len > 0 ? static_cast<uint8_t>(key[key_len - 1]) : 0;
    seg += key_len > 1 ? static_cast<uint8_t>(key[key_len - 2]) << 8 : 0;
    seg += key_len > 2 ? static_cast<uint8_t>(key[key_len - 3]) << 16 : 0;
    seg += key_len > 3 ? static_cast<uint8_t>(key[key_len - 4]) << 24 : 0;
    seg += (key_len > 4 ? static_cast<uint8_t>(key[key_len - 5]) : 0) * h_mult;
    return seg;
}

class StringMap {
public:
    StringMap(){

    }
    ~StringMap() = default;

    void insert(const char* key, size_t key_len, uint32_t value){
        uint64_t hash = suffix_hash(key, key_len);
        hash_buffer_[key_len] = hash;
    }

    const uint32_t* find(const char* key, size_t key_len) const{
        uint64_t hash = suffix_hash(key, key_len);
        return &buffer[hash % 16];
    }
    
private:
    uint64_t hash_buffer_[17];
    uint32_t buffer[17];
};
}