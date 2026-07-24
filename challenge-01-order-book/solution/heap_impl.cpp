#include "solution.h"
#include <unordered_set>
#include <queue>

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

    class Impl{
        public:
            Impl(){

            }

            inline void add_order(uint64_t id, int side, int64_t price, int64_t quantity){
                orders_.insert(id);
                if (side){ //asks
                    asks_.push({price, id});
                }
                else {
                    bids_.push({price, id});
                }
            }

            inline void cancel_order(uint64_t id){
                orders_.erase(id);

                while (!asks_.empty() && orders_.find(asks_.top().id) == orders_.end()){
                    asks_.pop();
                }

                while (!bids_.empty() && orders_.find(bids_.top().id) == orders_.end()){
                    bids_.pop();
                }
            }

            inline int64_t best_bid() const {
                if (bids_.empty()) return 0;
                return bids_.top().price;
            }

            inline int64_t best_ask() const{
                if (asks_.empty()) return 0;
                return asks_.top().price;
            }

        private: 
            std::unordered_set<uint64_t> orders_;
            std::priority_queue<Order, std::vector<Order>, Descending> bids_;
            std::priority_queue<Order, std::vector<Order>, Ascending> asks_;            
    };


    void OrderBook::add_order(uint64_t id, int side, int64_t price, int64_t quantity) {
        this->impl->add_order(id, side, price, quantity);
    }

    void OrderBook::cancel_order(uint64_t id) {
        this->impl->cancel_order(id);
    }

    int64_t OrderBook::best_bid() const {
        return this->impl->best_bid();
    }

    int64_t OrderBook::best_ask() const {
        return this->impl->best_ask();
    }   

    OrderBook::OrderBook(){
        this->impl = new Impl();
    }

    OrderBook::~OrderBook(){
        delete this->impl;   
    }
} 
