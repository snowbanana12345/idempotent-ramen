#pragma once


// #include "default_impl.h"
// #include "split_impl.h"
#include "boost_impl.h"


/*
class MultiOrderBook {
public:
    explicit MultiOrderBook(Venue& venue);

    // Our orders — forward to venue
    void send_order(uint64_t our_id, uint16_t symbol, int side,
                    int64_t price, int64_t qty);
    void modify_our_order(uint64_t our_id, int64_t new_price, int64_t new_qty);
    void cancel_our_order(uint64_t our_id);

    // Exchange feed — all orders (including ours when they arrive)
    void add_order(uint64_t exchange_id, uint16_t symbol, int side,
                   int64_t price, int64_t qty);
    void modify_order(uint64_t exchange_id, int64_t new_qty);  // qty-down only
    void cancel_order(uint64_t exchange_id);

    // Queries
    TopLevel best_bid(uint16_t symbol) const;
    TopLevel best_ask(uint16_t symbol) const;
    int get_top_levels(uint16_t symbol, int side, int n, TopLevel* out) const;
    int64_t volume_near_best(uint16_t symbol, int side, int64_t depth) const;
    QueuePosition get_queue_position(uint64_t our_id) const;
};
}
*/