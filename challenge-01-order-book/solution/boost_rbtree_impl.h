#include "base.h"
#include <boost/intrusive/rbtree.hpp>


namespace hftu {
    class OrderBook{
        public:
            OrderBook() = default;
            ~OrderBook() = default;

            void add_order(uint64_t id, int side, int64_t price, int64_t quantity){

            }

            void cancel_order(uint64_t id){

            }

            inline int64_t best_bid() const {
                
            }

            inline int64_t best_ask() const{
            
            }

        private: 
            
    };
} 
