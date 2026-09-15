
#include "base.h"

#include "fnv_hash.h"
#include <unordered_map>
#include <iostream>

namespace hftu {

struct Slot{
    uint32_t value;
    const char* key_ = nullptr;
    size_t key_len = 0;
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

        if (key_len == slot->key_len && std::memcmp(slot->key_, key, key_len)){ // slot already taken, store in L1 collision map
            map_.emplace(CachedStringKey(key, key_len, h), value);
        }
        else {
            slot->value = value;
            slot->key_ = key;
            slot->key_len = key_len;
        }
    }

    const uint32_t* find(const char* key, size_t key_len) const{
        if (key_len == 0) return has_zero ? &zero_slot : nullptr; 
        size_t h = hash_key(key, key_len);
        size_t main_ptr = h % BUFFER_SIZE;
        Slot* slot = &buffer_[main_ptr]; 

        if (slot->key_len == key_len && std::memcmp(key, slot->key_, key_len) == 0){ // key exist in this slot
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
    std::unordered_map<CachedStringKey, uint32_t, CachedStringKeyHash, CachedStringKeyEq> map_;
};
}