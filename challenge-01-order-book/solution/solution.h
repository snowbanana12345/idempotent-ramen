#pragma once

/*
Entry point file to switch between implementations, all of the headers implements the inferface below

class OrderBook {
public:
    OrderBook();
    ~OrderBook();
    void add_order(uint64_t id, int side, int64_t price, int64_t quantity);
    void cancel_order(uint64_t id);
    int64_t best_bid() const;
    int64_t best_ask() const;
};
*/

#include "stl_unordered_map_impl.h"
// #include "heap_impl.h"
// #include "bst_impl.h"
// #include "boost_rbtree_impl.h"
// #include "boost_treap_impl.h"
// #include "boost_dary_heap.h"