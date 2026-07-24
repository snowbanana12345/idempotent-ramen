#include "solution.h"

namespace hftu {
    struct Order {
        int side;
        int64_t price;
        int64_t quantity;
    };

    class Impl{
        public:
            Impl(){

            }

            inline void add_order(uint64_t id, int side, int64_t price, int64_t quantity){
                orders_[id] = {side, price, quantity};
                if (side == 0) {
                    bids_[price] += quantity;
                } else {
                    asks_[price] += quantity;
                }
            }
            inline void cancel_order(uint64_t id){
                auto it = orders_.find(id);
                if (it == orders_.end()) return;
                auto& order = it->second;
                if (order.side == 0) {
                    auto bit = bids_.find(order.price);
                    if (bit != bids_.end()) {
                        bit->second -= order.quantity;
                        if (bit->second <= 0) bids_.erase(bit);
                    }
                } else {
                    auto ait = asks_.find(order.price);
                    if (ait != asks_.end()) {
                        ait->second -= order.quantity;
                        if (ait->second <= 0) asks_.erase(ait);
                    }
                }
                orders_.erase(it);
            }
            inline int64_t best_bid() const {
                return bids_.empty() ? 0 : bids_.begin()->first;
            }
            inline int64_t best_ask() const{
                return asks_.empty() ? 0 : asks_.begin()->first;
            }
        private: 
            std::unordered_map<uint64_t, Order> orders_;
            std::map<int64_t, int64_t, std::greater<>> bids_;
            std::map<int64_t, int64_t> asks_;            
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
