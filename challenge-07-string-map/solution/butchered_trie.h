#pragma once
#include "base.h"

#include <vector>
#include <iostream>

namespace hftu {

struct Slot{
    const char* key;
    size_t key_len;
    uint32_t value;
    explicit Slot (const char* key, size_t key_len, uint32_t value) : key(key), key_len(key_len), value(value) {}
};

static size_t idx2(const char* k) {
        return (size_t(uint8_t(k[0])) << 8)
            |  size_t(uint8_t(k[1]));
}

class StringMap {
public:
    StringMap() = default;
    ~StringMap() = default;

    void insert(const char* key, size_t key_len, uint32_t value){
        if (key_len < 2){
            size_t ind = key_len > 0 ? static_cast<size_t>(key[0]) : 0;
            one_ptrs_[ind] = one_buffer_ + ind;
            one_buffer_[ind] = value;
        }
        else{
            size_t ind = idx2(key);
            slots_[ind].emplace_back(key, key_len, value);
        }
    }

    const uint32_t* find(const char* key, size_t key_len) const{
        if (key_len < 2){
            size_t ind = key_len > 0 ? static_cast<size_t>(key[0]) : 0;
            return one_ptrs_[ind];
        }
        else{
            size_t ind = idx2(key);

            for (const Slot& slot : slots_[ind]){
                if (slot.key_len == key_len && std::memcmp(slot.key, key, key_len) == 0){
                    return &slot.value;
                }
            }

            return nullptr;
        }
    }
    
private:
    uint32_t* one_ptrs_[256] = {nullptr};
    uint32_t* one_buffer_ = new uint32_t[256]();
    std::vector<Slot> slots_[256 * 256];
};
}
