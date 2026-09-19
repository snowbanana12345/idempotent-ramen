#include "../base.h"

// #include "stl_multi_map_small.h"
// #include "stl_vec_small.h"
#include "boost_static_vec_small.h"

namespace hftu {
    constexpr size_t HOT_SIZE = 16;

    struct Order {
        int64_t price;
        uint64_t id;

        Order(int64_t p, uint64_t i) : price(p), id(i) {}
    };

    struct Ascending {
        bool operator()(const Order &a, const Order &b) { return a.price < b.price; };
    };

    class OrderBook{
        public:
            OrderBook() = default;
            ~OrderBook() {

            }

            void add_order(uint64_t id, int side, int64_t price, int64_t quantity){
                price = (-1 + 2 * side) * price; 
                if (cold_orders_[side].empty() && books_[side].size() < HOT_SIZE){ // predictable branch, will not go into this branch after initialization
                    hot_sides_[id] = side;
                    books_[side].add_order(id, price);
                    return;
                }

                if (books_[side].size() > 0 && price < books_[side].worst()){
                    hot_sides_[id] = side;
                    books_[side].add_order(id, price);
                    if (books_[side].size() > HOT_SIZE){
                        SmallOrder popped = books_[side].pop_worst();
                        cold_orders_[side][popped.id] = price;
                    }
                }
                else {
                    cold_orders_[side][id] = price;
                }
            }

            void cancel_order(uint64_t id){
                auto it = hot_sides_.find(id);
                if (it != hot_sides_.end()){
                    bool side = it->second;
                    books_[side].cancel_order(id);
                    hot_sides_.erase(id);

                    if (!cold_orders_[side].empty() && books_[side].size() == 0){
                        pull_from_cold(side);
                    }

                    return;
                }

                cold_orders_[0].erase(id);
                cold_orders_[1].erase(id);
            }

            int64_t best_bid() const {
                return - books_[0].best();
            }

            int64_t best_ask() const{
                return books_[1].best();
            }

        private: 
            std::unordered_map<uint64_t, int64_t> cold_orders_[2]; // l3 cache  
            std::unordered_map<uint64_t, bool> hot_sides_; // l1 cache
            SmallBook books_[2]; // l1 cache , maybe spill a little bit into l2

            void pull_from_cold(int side){
                 // max heap, top of heap will be the worst price
                 // top K algorithm
                std::cout << "triggering rebuild" << std::endl;
                std::priority_queue<Order, std::vector<Order>, Ascending> rebuild_buffer_; 
               
                for (auto it = cold_orders_[side].begin(); it != cold_orders_[side].end(); it++){
                    rebuild_buffer_.push({it->second, it->first});
                    if (rebuild_buffer_.size() > HOT_SIZE){
                        rebuild_buffer_.pop();
                    }
                }

                while (!rebuild_buffer_.empty()){
                    Order order = rebuild_buffer_.top();
                    books_[side].add_order(order.id, order.price);
                    cold_orders_[side].erase(order.id);
                    rebuild_buffer_.pop();
                }
            }
    };
} 
