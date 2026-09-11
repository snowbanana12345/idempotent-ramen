#include "base.h"

namespace hftu {
    /*
    performance is reasonable. nodal structures are not cache friendly 
    each traversal is random memory access

    red-black tree based implementation of order book
    add_order(): O(log n)
    cancel_order(): O(log n)
    best_bid(): O(log n)
    best_ask(): O(log n)
    
    */

    struct Order {
        int side;
        int64_t price;
        int64_t quantity;
    };

    class OrderBook{
        public:
            OrderBook() = default;
            ~OrderBook() = default;

            void add_order(uint64_t id, int side, int64_t price, int64_t quantity){
                orders_[id] = {side, price, quantity};
                if (side == 0) {
                    bids_[price] += quantity;
                } else {
                    asks_[price] += quantity;
                }
            }
            void cancel_order(uint64_t id){
                auto it = orders_.find(id);
                if (it == orders_.end()) return;
                auto& order = it->second;
                if (order.side == 0) {
                    auto bit = bids_.find(order.price);
                    if (bit != bids_.end()) {
                        bit->second -= order.quantity;
                        if (bit->second <= 0) bids_.erase(bit);
                    }
                } else {
                    auto ait = asks_.find(order.price);
                    if (ait != asks_.end()) {
                        ait->second -= order.quantity;
                        if (ait->second <= 0) asks_.erase(ait);
                    }
                }
                orders_.erase(it);
            }

            int64_t best_bid() const {
                return bids_.empty() ? 0 : bids_.begin()->first;
            }

            int64_t best_ask() const{
                return asks_.empty() ? 0 : asks_.begin()->first;
            }
        private: 
            std::unordered_map<uint64_t, Order> orders_;
            std::map<int64_t, int64_t, std::greater<>> bids_;
            std::map<int64_t, int64_t> asks_;            
    };
} 
