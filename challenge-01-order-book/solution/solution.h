#pragma once
// Challenge 01: Order Book
// Edit this file and solution.cpp to implement your solution.

#include <cstdint>
#include <map>
#include <unordered_map>

namespace hftu {
    class Impl;
    
    class OrderBook {
    public:
        OrderBook();
        ~OrderBook();
        void add_order(uint64_t id, int side, int64_t price, int64_t quantity);
        void cancel_order(uint64_t id);
        int64_t best_bid() const;
        int64_t best_ask() const;

    private:
        Impl* impl;
    };
}
