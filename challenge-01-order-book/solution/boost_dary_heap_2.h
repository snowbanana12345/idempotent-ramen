#pragma once

#include <boost/heap/d_ary_heap.hpp>
#include <unordered_set>

namespace hftu{
    /* 
    production viable implementation where we actually remove the deleted orders from the cache
    same time complexities, performance is worse since we have to avoid the worst case of orders stacking up.
    */

    struct Order {
        uint64_t id;
        int64_t price;

        Order(int64_t p, uint64_t i) : price(p), id(i) {}
    };

    struct Comp {
    bool operator()(const Order& a, const Order& b) const {
            return a.price > b.price; 
        }
    };

    using Heap = boost::heap::d_ary_heap<Order, boost::heap::arity<4>, boost::heap::compare<Comp>, boost::heap::mutable_<true>>;

    struct Entry{
        Heap::handle_type handle;
        int side;
    };

    class OrderBook {
    public:
        

        OrderBook() = default;

        void add_order(uint64_t id, int side, int64_t price, int64_t qty) {
            
            int sign = -1 + 2 * side;
            Heap::handle_type handle = books_[side].push(Order(sign * price, id));
            orders_.emplace(id, Entry{handle, side});
        }

        void cancel_order(uint64_t id) {
            auto it = orders_.find(id);
            if (it != orders_.end()) {
                books_[it->second.side].erase(it->second.handle);
                orders_.erase(it);
            }
        }

        int64_t best_bid() const {
            if (books_[0].empty()) return 0; // evaluates to false > 99% of the time
            return - books_[0].top().price;
        }

        int64_t best_ask() const{
            if (books_[1].empty()) return 0; // evaluates to false > 99% of the time
            return books_[1].top().price;
        }
        
    private:
        mutable Heap books_[2]; 
        std::unordered_map<uint64_t, Entry> orders_;
    };
} 