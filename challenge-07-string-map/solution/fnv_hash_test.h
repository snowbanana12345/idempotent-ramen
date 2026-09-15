#include "base.h"

#include "fnv_hash.h"

namespace hftu {

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