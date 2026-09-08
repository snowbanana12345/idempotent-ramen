#include "base.h"
#include <boost/intrusive/treap_set.hpp>


namespace hftu {
    class Order : public boost::intrusive::bs_set_base_hook<> 
    {
        public:
            boost::intrusive::bs_set_member_hook<> member_hook_;
            uint64_t order_id_;
            int64_t price_;

            Order() = default;

            friend bool operator< (const Order &a, const Order &b)
                {  return a.order_id_ < b.order_id_;  }

            friend bool operator> (const Order &a, const Order &b)
                {  return a.order_id_ > b.order_id_;  }
    };

    struct OrderLess {
        bool operator()(const Order& a, const Order& b) const {
            return a.order_id_ < b.order_id_;
        }
    };

    struct Descending {
        using type = int64_t;
        type operator()(const Order& o) {
            return - o.price_;
        }
    };

    struct Ascending {
        using type = int64_t;
        type operator()(const Order& o) {
            return o.price_;
        }
    };

    constexpr uint32_t EXPECTED_SIZE = 1'000'000;

    using TreapBid = boost::intrusive::treap_set<Order, boost::intrusive::compare<OrderLess>, boost::intrusive::priority_of_value<Descending>>;
    using TreapAsk = boost::intrusive::treap_set<Order, boost::intrusive::compare<OrderLess>, boost::intrusive::priority_of_value<Ascending>>;

    class OrderBook{
        public:
            OrderBook() {
                pool_.reserve(EXPECTED_SIZE);
                for (int i = 0; i < EXPECTED_SIZE; i++){
                    pool_.push_back(new Order());
                }
            }

            ~OrderBook() = default;

            void add_order(uint64_t id, int side, int64_t price, int64_t quantity){
                Order* new_order = pool_.back();
                pool_.pop_back();
                new_order->order_id_ = id;
                new_order->price_ = price;

                if (side){
                    asks_.insert(*new_order);
                }
                else{
                    bids_.insert(*new_order);
                }
            }

            void cancel_order(uint64_t id){
                dummy_.order_id_ = id;
                auto handle = bids_.find(dummy_);
                if (handle != bids_.end()) {
                    bids_.erase(handle);
                    Order* order = &*handle;
                    pool_.push_back(order);
                    return;
                }

                handle = asks_.find(dummy_);
                if (handle != asks_.end()) {
                    asks_.erase(handle);
                    Order* order = &*handle;
                    pool_.push_back(order);
                    return;
                }
            }

            int64_t best_bid() const {
                auto it = bids_.root();
                return it != bids_.end() ? it->price_ : 0;
            }

            int64_t best_ask() const{
                auto it = asks_.root();
                return it != asks_.end() ? it->price_ : 0;
            }

        private: 
            std::vector<Order*> pool_;
            Order dummy_;
            TreapBid bids_;
            TreapAsk asks_;
    };
} 
