#include "base.h"
#include <boost/intrusive/rbtree.hpp>


namespace hftu {
    class Level : public boost::intrusive::set_base_hook<boost::intrusive::optimize_size<true> >
    {
    public:
        Level() = default;
        Level(int64_t price) : price_(price) {};

        friend bool operator< (const Level &a, const Level &b)
            {  return a.price_ < b.price_;  }
        friend bool operator> (const Level &a, const Level &b)
            {  return a.price_ > b.price_;  }
        friend bool operator== (const Level &a, const Level &b)
            {  return a.price_ == b.price_;  }

        int64_t price_;
        size_t count_;
    };

    struct Order{
        int64_t price;
        int side;
    };

    constexpr uint32_t EXPECTED_SIZE = 1'000'000;

    class OrderBook{
        public:
            OrderBook() {
                orders_.reserve(1'000'000);
                for (int i = 0; i < EXPECTED_SIZE; i++){
                    level_pool.push_back(new Level());
                }
            }
            ~OrderBook() = default;

            void add_order(uint64_t id, int side, int64_t price, int64_t quantity){
                boost::intrusive::rbtree<Level>& tree_ = side ? asks_ : bids_;

                orders_.emplace(id, Order{price, side});
                Level key(price);
                auto it = tree_.find(key);
                if (it != tree_.end()) {
                    it->count_++;
                }
                else {
                    Level* new_level = level_pool.back();
                    level_pool.pop_back();
                    new_level->price_ = price;
                    new_level->count_ = 1;
                    tree_.insert_unique(*new_level);
                }
            }

            void cancel_order(uint64_t id){
                auto oit = orders_.find(id);
                if (oit == orders_.end()) return;

                int64_t price = oit->second.price;
                int side = oit->second.side;

                orders_.erase(oit);
               
                boost::intrusive::rbtree<Level>& tree_ = side ? asks_ : bids_;
                Level key(price);
                auto it = tree_.find(key);
                it->count_--;
                if (it->count_ == 0){
                    Level* level_ptr = &*it;  // Dereference iterator, then take address
                    level_pool.push_back(level_ptr); // return level back to pool
                    tree_.erase(it); // drop the pointer from tree;
                }
            }

            int64_t best_bid() const {
                if (bids_.empty()) return 0;
                return bids_.rbegin()->price_;
            }

            int64_t best_ask() const{
                if (asks_.empty()) return 0;
                return asks_.begin()->price_;
            }

        private: 
            std::unordered_map<uint64_t, Order> orders_;
            std::vector<Level*> level_pool;
            boost::intrusive::rbtree<Level> asks_;
            boost::intrusive::rbtree<Level> bids_;
    };
} 
