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
        std::fill(std::begin(one_value), std::end(one_value), nullptr);
        std::fill(std::begin(two_value), std::end(two_value), nullptr);
    }

    ~StringMap() = default;

    void insert(const char* key, size_t key_len, uint32_t value){
        switch (key_len) {
            case 0:
                zero_value = new uint32_t(value);
                break;
            case 1:
                one_value[idx1(key)] = new uint32_t(value);
                break;
            case 2:
                two_value[idx2(key)] = new uint32_t(value);
                break;
            default:
                map_.emplace(std::string(key, key_len), value);
        }
    }

    const uint32_t* find(const char* key, size_t key_len) const{
        switch(key_len) {
            case 0:
                return zero_value;
            case 1:
                return one_value[idx1(key)];
            case 2:
                return two_value[idx2(key)];
            default:
                auto it = map_.find(std::string(key, key_len));
                if (it == map_.end()) return nullptr;
                return &it->second;
        }
    }
    
private:
    uint32_t* zero_value;
    uint32_t* one_value[256];
    uint32_t* two_value[256 * 256];
    std::unordered_map<std::string, uint32_t> map_;
};
}


