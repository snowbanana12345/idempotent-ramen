#include "base.h"


namespace hftu {

/* speed test for computing FNV hash key 
    the fnv key itself comsumes 7.5 cycles/op on average
*/

int hash_key(const char* key, size_t key_len) {
    uint64_t h = 1469598103934665603ull;       
    for (size_t i = 0; i < key_len; ++i) {
        h ^= static_cast<uint8_t>(key[i]);
        h *= 1099511628211ull;                  
    }
    return h;
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