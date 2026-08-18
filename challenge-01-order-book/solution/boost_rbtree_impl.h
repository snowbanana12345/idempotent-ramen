#include "base.h"
#include <boost/intrusive/rbtree.hpp>


namespace hftu {
    class Level : public boost::intrusive::set_base_hook<boost::intrusive::optimize_size<true> >
    {
    int64_t price_;

    public:
        boost::intrusive::set_member_hook<> member_hook_;

        Level(int64_t price)
            :  price_(price)
            {}
        friend bool operator< (const Level &a, const Level &b)
            {  return a.price_ < b.price_;  }
        friend bool operator> (const Level &a, const Level &b)
            {  return a.price_ > b.price_;  }
        friend bool operator== (const Level &a, const Level &b)
            {  return a.price_ == b.price_;  }

        int64_t get_price() const{
            return price_;
        }
    };

    class OrderBook{
        public:
            OrderBook() = default;
            ~OrderBook() = default;

            void add_order(uint64_t id, int side, int64_t price, int64_t quantity){
                if (!side){
                    auto [it, inserted] = bid_orders_.emplace(id, price);
                    bids_.insert_unique(it->second);
                }
                else {
                    auto [it, inserted] =  ask_orders_.emplace(id, price);
                    asks_.insert_unique(it->second);
                }
            }

            void cancel_order(uint64_t id){
                auto bid_it = bid_orders_.find(id);
                if (bid_it != bid_orders_.end()){
                    bids_.erase(bid_it->second); // remove reference from tree
                    bid_orders_.erase(bid_it); 
                    return;
                }
                auto ask_it = ask_orders_.find(id);
                if (ask_it != ask_orders_.end()){
                    asks_.erase(ask_it->second); // remove reference from tree
                    ask_orders_.erase(ask_it); 
                    return;
                }
            }

            int64_t best_bid() const {
                if (bids_.empty()) return 0;
                std::cout << "bid is not empty" << std::endl;
                return bids_.rbegin()->get_price();
            }

            int64_t best_ask() const{
                if (asks_.empty()) return 0;
                return asks_.begin()->get_price();
            }

        private: 
            // maps hold ownership of the Level objects
            std::unordered_map<uint64_t, Level> ask_orders_; 
            std::unordered_map<uint64_t, Level> bid_orders_;

            // intrusive trees just hold references to objects
            boost::intrusive::rbtree<Level> asks_;
            boost::intrusive::rbtree<Level> bids_;
    };
} 
