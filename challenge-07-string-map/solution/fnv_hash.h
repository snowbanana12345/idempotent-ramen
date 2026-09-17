#include "base.h"

namespace hftu{

constexpr uint64_t h_init = 1469598103934665603ull;
constexpr uint64_t h_mult = 1099511628211ull;

size_t hash_key(const char* key, size_t key_len) {
    size_t seg = key_len > 0 ? static_cast<uint8_t>(key[key_len - 1]) : 0;
    seg += key_len > 1 ? static_cast<uint8_t>(key[key_len - 2]) << 8 : 0;
    seg += key_len > 2 ? static_cast<uint8_t>(key[key_len - 3]) << 16 : 0;
    seg += key_len > 3 ? static_cast<uint8_t>(key[key_len - 4]) << 24 : 0;
    seg += (key_len > 4 ? static_cast<uint8_t>(key[key_len - 5]) : 0) * h_mult;
    return seg ^ key_len * h_mult;
}

struct StringKey {
    const char* data;
    uint32_t    len;

    StringKey(const char* d, uint32_t l) : data(d), len(l) {}
};

template <size_t (*HashFn)(const char*, size_t)>
struct StringKeyHash {
    size_t operator()(const StringKey& k) const noexcept {
        return static_cast<size_t>(HashFn(k.data, k.len));
    }
};

struct StringKeyEq {
    bool operator()(const StringKey& a, const StringKey& b) const noexcept {
        return a.len == b.len && std::memcmp(a.data, b.data, a.len) == 0;
    }
};
}