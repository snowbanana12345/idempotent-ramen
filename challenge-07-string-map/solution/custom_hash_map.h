
#include "base.h"

#include "fnv_hash.h"
#include <unordered_map>
#include <iostream>

namespace hftu {

struct Slot{ // pass the 32-byte threshold
    uint32_t value;
    char key_[16] = {0};
};

constexpr size_t BUFFER_SIZE = 1024 * 1024;

class StringMap {
public:
    StringMap() = default;
    ~StringMap() = default;

    void insert(const char* key, size_t key_len, uint32_t value){
        if (key_len == 0 && !has_zero){
            has_zero = true;
            zero_slot = value;
        }
        size_t h = hash_key(key, key_len);
        size_t main_ptr = h % BUFFER_SIZE;
        Slot* slot = &buffer_[main_ptr]; 

        char tmp[16] = {0};      
        std::memcpy(tmp, key, key_len); 

        if (memcmp(tmp, slot->key_, 16)){ // slot already taken, store in L1 collision map
            map_.emplace(CachedStringKey(key, key_len, h), value);
        }
        else {
            std::memcpy(slot->key_, tmp, 16);
            slot->value = value;
        }
        
    }

    const uint32_t* find(const char* key, size_t key_len) const{
        if (key_len == 0) return has_zero ? &zero_slot : nullptr; 
        size_t h = hash_key(key, key_len);
        size_t main_ptr = h % BUFFER_SIZE;
        Slot* slot = &buffer_[main_ptr]; 

        char tmp[16] = {0};      
        std::memcpy(tmp, key, key_len); 

        if (std::memcmp(tmp, slot->key_, 16) == 0){ // key exist in this slot
            return &slot->value;
        }
        // search the collision map to see if key exists
        auto it = map_.find(CachedStringKey(key, key_len, h));
        if (it == map_.end()) return nullptr;
        return &it->second;
    }
    
private:
    bool has_zero = false;
    uint32_t zero_slot;
    Slot* buffer_ = new Slot[BUFFER_SIZE];
    std::unordered_map<CachedStringKey, uint32_t, CachedStringKeyHash, CachedStringKeyEq> map_; // very small, should reside in L1 cache
};
}