#include "../base.h"

// #include "stl_multi_map_small.h"
// #include "stl_vec_small.h"
#include "boost_static_vec_small.h"

#include "cold_book.h"

namespace hftu {
    constexpr size_t HOT_SIZE = 16;


    class OrderBook{
        public:
            OrderBook() = default;
            ~OrderBook() {

            }

            void add_order(uint64_t id, int side, int64_t price, int64_t quantity){
                price = (-1 + 2 * side) * price; 
                if (cold_.empty(side) && books_[side].size() < HOT_SIZE){ 
                    // predictable branch, will not go into this branch after initialization
                    hot_sides_[id] = side;
                    books_[side].add_order(id, price);
                    return;
                }

                if (books_[side].size() > 0 && price < books_[side].worst()){
                    hot_sides_[id] = side;
                    books_[side].add_order(id, price);
                    if (books_[side].size() > HOT_SIZE){
                        Order popped = books_[side].pop_worst();
                        cold_.add_order(popped.id, popped.price, side);
                    }
                }
                else {
                    cold_.add_order(id, price, side);
                }
            }

            void cancel_order(uint64_t id){
                auto it = hot_sides_.find(id);
                if (it != hot_sides_.end()){ // triggers 1% of the time as the hot data holds 1% of the levels
                    bool side = it->second;
                    books_[side].cancel_order(id);
                    hot_sides_.erase(id);

                    if (!cold_.empty(side) && books_[side].size() == 0){ // should only trigger once in am illion
                        std::vector<Order> cold_orders_ = cold_.pull(side, HOT_SIZE);
                        for (const Order& order : cold_orders_){
                            books_[side].add_order(order.id, order.price);
                        }
                    }

                    return;
                }

                cold_.cancel_order(id);
            }

            int64_t best_bid() const {
                return - books_[0].best();
            }

            int64_t best_ask() const{
                return books_[1].best();
            }

        private: 
            ColdBook cold_;
            std::unordered_map<uint64_t, bool> hot_sides_; // l1 cache
            SmallBook books_[2]; // l1 cache , maybe spill a little bit into l2
    };
} 
