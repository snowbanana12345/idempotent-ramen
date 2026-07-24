#include "solution.h"
#include <unordered_map>
#include <algorithm>

namespace hftu {
    class Impl{
        public:
            Impl(){

            }

            inline void add_order(uint64_t id, int side, int64_t price, int64_t quantity){
                bids_.insert({id, price});
                asks_.insert({id, price});
            }

            inline void cancel_order(uint64_t id){
                bids_.erase(id);
                asks_.erase(id);
            }

            inline int64_t best_bid() const {
                if (bids_.empty()) return 0;
                auto max_it = std::max_element(
                    bids_.begin(), 
                    bids_.end(),
                    [](const auto& a, const auto& b) {
                        return a.second < b.second;
                    }
                );
                return max_it->second;
            }

            inline int64_t best_ask() const{
                if (asks_.empty()) return 0;
                auto max_it = std::min_element(
                    asks_.begin(), 
                    asks_.end(),
                    [](const auto& a, const auto& b) {
                        return a.second < b.second;
                    }
                );
                return max_it->second;
            }

        private: 
            std::unordered_map<uint64_t, int64_t> bids_;
            std::unordered_map<uint64_t, int64_t> asks_;          
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
