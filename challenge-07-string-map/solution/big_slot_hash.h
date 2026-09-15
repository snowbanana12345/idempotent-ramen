#include "base.h"

#include "fnv_hash.h"
#include <unordered_map>
#include <iostream>

namespace hftu {

struct SmallSlot{
    uint32_t value;
    char key_[8] = {0};
};

struct BigSlot{
    uint32_t value;
    char key_[16] = {0};
};

constexpr size_t BUFFER_SIZE = 512 * 1024;

class StringMap {
public:
    StringMap() = default;
    ~StringMap() = default;

    void insert(const char* key, size_t key_len, uint32_t value){
        if (key_len == 0 && !has_zero){
            has_zero = true;
            zero_slot = value;
        }
        
        else if (key_len < 9){
            uint64_t h = hash_key_8(key, key_len);
            size_t main_ptr = h % BUFFER_SIZE;
            SmallSlot* slot = &small_buffer_[main_ptr]; 

            // slot already taken, store in L1 collision map, predicable branch to be false
            if (std::strlen(slot->key_) == key_len && std::memcmp(slot->key_, key, key_len)){ 
                map_.emplace(CachedStringKey(key, key_len, h), value);
            }
            else {
                std::memcpy(slot->key_, key, key_len);
                slot->value = value;   
            }
        }

        else{
            uint64_t h = hash_key(key, key_len);
            size_t main_ptr = h % BUFFER_SIZE;
            BigSlot* slot = &big_buffer_[main_ptr]; 

            // slot already taken, store in L1 collision map, predicable branch to be false
            if (std::strlen(slot->key_) == key_len && std::memcmp(slot->key_, key, key_len)){ 
                map_.emplace(CachedStringKey(key, key_len, h), value);
            }
            else {
                std::memcpy(slot->key_, key, key_len);
                slot->value = value;   
            }
        }
    }

    const uint32_t* find(const char* key, size_t key_len) const{
        if (key_len == 0) return has_zero ? &zero_slot : nullptr; 

        else if (key_len < 9){
            uint64_t h = hash_key_8(key, key_len);
            size_t main_ptr = h % BUFFER_SIZE;
            SmallSlot* slot = &small_buffer_[main_ptr]; 

            if (std::strlen(slot->key_) == key_len && std::memcmp(slot->key_, key, key_len) == 0){ 
                return &slot->value;
            }

            auto it = map_.find(CachedStringKey(key, key_len, h));
            if (it == map_.end()) return nullptr;
            return &it->second;
        }
        
        else {
            uint64_t h = hash_key(key, key_len);
            size_t main_ptr = h % BUFFER_SIZE;
            BigSlot* slot = &big_buffer_[main_ptr]; 

            if (std::strlen(slot->key_) == key_len && std::memcmp(slot->key_, key, key_len) == 0){ 
                return &slot->value;
            }

            auto it = map_.find(CachedStringKey(key, key_len, h));
            if (it == map_.end()) return nullptr;
            return &it->second;
        }
    }
    
private:
    bool has_zero = false;
    uint32_t zero_slot;
    SmallSlot* small_buffer_ = new SmallSlot[BUFFER_SIZE];
    BigSlot* big_buffer_ = new BigSlot[BUFFER_SIZE];
    std::unordered_map<CachedStringKey, uint32_t, CachedStringKeyHash, CachedStringKeyEq> map_;
};
}