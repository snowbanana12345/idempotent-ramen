#include <cstdint>
#include <cstddef>
#include <mutex>
#include <vector>

struct Message {
    uint64_t timestamp;   // 8
    uint32_t symbol_id;   // 4
    uint16_t side;        // 2
    uint16_t flags;       // 2
    int64_t  price;       // 8
    int64_t  quantity;    // 8
    int64_t  order_id;   // 8
    uint64_t sequence;    // 8
};                        // 48 bytes total