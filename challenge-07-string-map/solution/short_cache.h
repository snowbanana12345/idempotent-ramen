# pragma once
#include "base.h"

#include <unordered_map>

namespace hftu {

static size_t idx1(const char* k) {
        return size_t(uint8_t(k[0]));
}

static size_t idx2(const char* k) {
        return (size_t(uint8_t(k[0])) << 8)
            |  size_t(uint8_t(k[1]));
}

static size_t idx3(const char* k) {
        return (size_t(uint8_t(k[0])) << 16)
            | (size_t(uint8_t(k[1])) <<  8)
            |  size_t(uint8_t(k[2]));
}

class StringMap {
public:
    StringMap(){
        map_.reserve(1024 * 1024);
    }

    ~StringMap() = default;

    void insert(const char* key, size_t key_len, uint32_t value){
        if (key_len < 3){
            size_t len_offset = key_len > 1 ? 256 : 0;
            size_t idx = len_offset + (key_len > 1 ? idx2(key) : idx1(key));
            short_ptr_[idx] = short_buf_ + alloc_idx_++;
            *short_ptr_[idx] = value;
        }
        else {
            map_.emplace(std::string(key, key_len), value);
        }
    }

    const uint32_t* find(const char* key, size_t key_len) const{
        if (key_len < 3){
            size_t len_offset = key_len > 1 ? 256 : 0;
            size_t idx = len_offset + (key_len > 1 ? idx2(key) : idx1(key));
            return short_ptr_[idx];
        }
        else {
            auto it = map_.find(std::string(key, key_len));
            if (it == map_.end()) return nullptr;
            return &it->second;
        }
    }
    
private:
    uint32_t* short_ptr_[256 * 256];
    uint32_t* short_buf_ = new uint32_t[256 * 256]();
    size_t alloc_idx_ = 0;
    std::unordered_map<std::string, uint32_t> map_;
};
}


