#include "base.h"


namespace hftu {
    struct Order {
        int64_t price;
        uint64_t id;
    };

    struct Ascending {
        bool operator()(const Order &a, const Order &b) { return a.price > b.price; };
    };

    struct Descending {
        bool operator()(const Order &a, const Order &b) { return a.price < b.price; };
    };

    class OrderBook{
        public:
            OrderBook() = default;
            ~OrderBook() = default;

            void add_order(uint64_t id, int side, int64_t price, int64_t quantity){
                orders_.insert(id);
                if (side){ //asks
                    asks_.push({price, id});
                }
                else {
                    bids_.push({price, id});
                }
            }

            void cancel_order(uint64_t id){
                orders_.erase(id);

                while (!asks_.empty() && orders_.find(asks_.top().id) == orders_.end()){
                    asks_.pop();
                }

                while (!bids_.empty() && orders_.find(bids_.top().id) == orders_.end()){
                    bids_.pop();
                }
            }

            int64_t best_bid() const {
                if (bids_.empty()) return 0;
                return bids_.top().price;
            }

            int64_t best_ask() const{
                if (asks_.empty()) return 0;
                return asks_.top().price;
            }

        private: 
            std::unordered_set<uint64_t> orders_;
            std::priority_queue<Order, std::vector<Order>, Descending> bids_;
            std::priority_queue<Order, std::vector<Order>, Ascending> asks_;            
    };
} 
