#include <cstdint>
#include <cstddef>
#include <mutex>
#include <vector>
#include <iostream>
#include <atomic>
#include <cassert>

namespace hftu{
    struct Message {
        uint64_t timestamp;   // 8
        uint32_t symbol_id;   // 4
        uint16_t side;        // 2
        uint16_t flags;       // 2
        int64_t  price;       // 8
        int64_t  quantity;    // 8
        int64_t  order_id;   // 8
        uint64_t sequence;    // 8

        bool operator==(const Message& other) const {  // Add 'const' for const-correctness
            return timestamp == other.timestamp &&
                symbol_id == other.symbol_id &&
                side == other.side &&
                flags == other.flags &&
                price == other.price &&
                quantity == other.quantity &&
                order_id == other.order_id &&
                sequence == other.sequence;
        }

        bool operator!=(const Message& other) const {
            return !(*this == other);  // Reuse operator==
        }
    };                        // 48 bytes total
}
