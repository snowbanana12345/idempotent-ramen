#pragma once
#include "base.h"

#include <unordered_map>

namespace hftu {

class StringMap {
public:
    StringMap() = default;
    ~StringMap() = default;

    void insert(const char* key, size_t key_len, uint32_t value){
        map_.emplace(std::string(key, key_len), value);
    }

    const uint32_t* find(const char* key, size_t key_len) const{
        auto it = map_.find(std::string(key, key_len));
        if (it == map_.end()) return nullptr;
        return &it->second;
    }
    
private:
    std::unordered_map<std::string, uint32_t> map_;
};

}
