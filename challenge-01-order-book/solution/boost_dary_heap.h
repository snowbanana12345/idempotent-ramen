#pragma once

#include <boost/heap/d_ary_heap.hpp>
#include <unordered_set>

namespace hftu{
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

    class OrderBook {
    public:
        using Heap = boost::heap::d_ary_heap<Order, boost::heap::arity<64>, boost::heap::compare<Comp>, boost::heap::mutable_<false>>;

        OrderBook() = default;

        void add_order(int64_t id, int side, double price, int64_t qty) {
            orders_.insert(id);
            int sign = -1 + 2 * side;
            books_[side].emplace(sign * price, id);
        }

        void cancel_order(int64_t id) {
            orders_.erase(id);
        }

        int64_t best_bid() const {
            while (!books_[0].empty() && orders_.find(books_[0].top().id) == orders_.end()){
                    books_[0].pop();
            }
            if (books_[0].empty()) return 0;
            return - books_[0].top().price;
        }

        int64_t best_ask() const{
            while (!books_[1].empty() && orders_.find(books_[1].top().id) == orders_.end()){
                    books_[1].pop();
            }
            if (books_[1].empty()) return 0;
            return books_[1].top().price;
        }
        
    private:
        mutable Heap books_[2]; 
        std::unordered_set<uint64_t> orders_;
    };
} 