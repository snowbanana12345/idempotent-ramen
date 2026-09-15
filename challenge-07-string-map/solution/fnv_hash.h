#include "base.h"

namespace hftu{

constexpr uint64_t h_init = 1469598103934665603ull;
constexpr uint64_t h_mult = 1099511628211ull;

uint64_t hash_key(const char* key, size_t key_len) {   

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

uint64_t hash_key_8(const char* key, size_t key_len) {   
    uint64_t seg_1 = key_len > 0 ? static_cast<uint8_t>(key[0]) : 0;
    seg_1 += key_len > 1 ? static_cast<uint8_t>(key[1]) << 8 : 0;
    seg_1 += key_len > 2 ? static_cast<uint8_t>(key[2]) << 16 : 0;
    seg_1 += key_len > 3 ? static_cast<uint8_t>(key[3]) << 24 : 0;

    uint64_t seg_2 = key_len > 4 ? static_cast<uint8_t>(key[4]) : 0;
    seg_2 += key_len > 5 ? static_cast<uint8_t>(key[5]) << 8 : 0;
    seg_2 += key_len > 6 ? static_cast<uint8_t>(key[6]) << 16 : 0;
    seg_2 += key_len > 7 ? static_cast<uint8_t>(key[7]) << 24 : 0;

    return seg_1 * h_mult ^ seg_2;
}

struct StringKey {
    const char* data;
    uint32_t    len;

    StringKey(const char* d, uint32_t l) : data(d), len(l) {}
};

struct StringKeyHash {
    size_t operator()(const StringKey& k) const noexcept {
        return static_cast<size_t>(hash_key(k.data, k.len));
    }
};

struct StringKeyEq {
    bool operator()(const StringKey& a, const StringKey& b) const noexcept {
        return a.len == b.len && std::memcmp(a.data, b.data, a.len) == 0;
    }
};

struct CachedStringKey {
    const char* data;
    uint32_t    len;
    size_t hash_;

    CachedStringKey(const char* d, uint32_t l, size_t hash_) : data(d), len(l), hash_(hash_) {}
};

struct CachedStringKeyHash {
    size_t operator()(const CachedStringKey& k) const noexcept {
        return k.hash_;
    }
};

struct CachedStringKeyEq {
    bool operator()(const CachedStringKey& a, const CachedStringKey& b) const noexcept {
        return a.len == b.len && std::memcmp(a.data, b.data, a.len) == 0;
    }
};
}