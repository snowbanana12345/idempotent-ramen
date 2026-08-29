#include "base.h"


namespace hftu {
    struct Order {
        int64_t price;
        uint64_t id;

        Order(int64_t p, uint64_t i) : price(p), id(i) {}
    };

    struct Ascending {
        bool operator()(const Order &a, const Order &b) { return a.price > b.price; };
    };

    class OrderBook{
        public:
            OrderBook() = default;
            ~OrderBook() = default;

            void add_order(uint64_t id, int side, int64_t price, int64_t quantity){
                orders_.insert(id);
                int sign = -1 + 2 * side; 
                books_[side].emplace(sign * price, id);
            }

            void cancel_order(uint64_t id){
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
            std::unordered_set<uint64_t> orders_;
            mutable std::priority_queue<Order, std::vector<Order>, Ascending> books_[2];         
    };
} 
