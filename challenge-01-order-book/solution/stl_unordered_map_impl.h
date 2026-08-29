#include "base.h"
#include <unordered_map>
#include <algorithm>

namespace hftu {
    class OrderBook{
        public:
            OrderBook() = default;
            ~OrderBook() = default;

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
} 
