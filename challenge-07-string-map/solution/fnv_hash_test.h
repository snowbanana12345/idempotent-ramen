#include "base.h"


namespace hftu {

/* speed test for computing FNV hash key 
    the fnv key itself comsumes 7.5 cycles/op on average
*/

constexpr uint64_t h_init = 1469598103934665603ull;
constexpr uint64_t h_mult = 1099511628211ull;

int hash_key(const char* key, size_t key_len) {   

    uint64_t seg_1 = key_len > 0 ? static_cast<uint8_t>(key[0]) : 0;
    seg_1 += key_len > 1 ? static_cast<uint8_t>(key[1]) << 8 : 0;
    seg_1 += key_len > 2 ? static_cast<uint8_t>(key[2]) << 16 : 0;
    seg_1 += key_len > 3 ? static_cast<uint8_t>(key[3]) << 24 : 0;

    uint64_t seg_2 = key_len > 4 ? static_cast<uint8_t>(key[4]) : 0;
    seg_2 += key_len > 5 ? static_cast<uint8_t>(key[5]) << 8 : 0;
    seg_2 += key_len > 6 ? static_cast<uint8_t>(key[6]) << 16 : 0;
    seg_2 += key_len > 7 ? static_cast<uint8_t>(key[7]) << 24 : 0;

    uint64_t seg_12 = seg_1 * h_mult ^ seg_2;

    uint64_t seg_3 = key_len > 8 ? static_cast<uint8_t>(key[8]) : 0;
    seg_3 += key_len > 9 ? static_cast<uint8_t>(key[9]) << 8 : 0;
    seg_3 += key_len > 10 ? static_cast<uint8_t>(key[10]) << 16 : 0;
    seg_3 += key_len > 11 ? static_cast<uint8_t>(key[11]) << 24 : 0;

    uint64_t seg_4 = key_len > 12 ? static_cast<uint8_t>(key[12]) : 0;
    seg_4 += key_len > 13 ? static_cast<uint8_t>(key[13]) << 8 : 0;
    seg_4 += key_len > 14 ? static_cast<uint8_t>(key[14]) << 16 : 0;
    seg_4 += key_len > 15 ? static_cast<uint8_t>(key[15]) << 24 : 0;

    uint64_t seg_34 = seg_3 * h_mult ^ seg_4;

    uint64_t h = seg_34 * h_mult ^ seg_12;

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