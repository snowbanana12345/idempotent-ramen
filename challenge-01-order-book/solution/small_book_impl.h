#include "base.h"


namespace hftu {
    /*
    a priority_queue tracks the best bid/ask
    hash map tracks the active orders

    add_order():O(log n) 
    cancel_order(): O(1)
    best_bid(): O(log n)
    best_ask(): O(log n)

    disclaimber : this solution works very well on the workload.
    But is a scenario where a lot of invalidated orders pile up in the pq, leading to infinity memory usage
    */

    constexpr size_t HOT_SIZE = 128;

    struct Order {
        int64_t price;
        uint64_t id;

        Order(int64_t p, uint64_t i) : price(p), id(i) {}
    };

    struct Ascending {
        bool operator()(const Order &a, const Order &b) { return a.price < b.price; };
    };


    class SmallBook{
        // this whole thing has to fit into L1 cache
        public:
            SmallBook() = default;
            ~SmallBook() = default;

            void add_order(uint64_t id, int64_t price){
                orders_[id] = price;
                book_.insert(price);
            }

            void cancel_order(uint64_t id){
                auto it = orders_.find(id);
                if (it == orders_.end()) return; // predictable, will always skip for the dataset
                auto b_it = book_.find(it->second);
                if (b_it != book_.end()) book_.erase(b_it); // predictable, will always be true for the dataset
                orders_.erase(it);
            }

            int64_t best() const {
                if (book_.empty()) return 0;
                return *book_.cbegin();
            }

            int64_t worst() const {
                if (book_.empty()) return 0;
                return *book_.crbegin();
            }

            uint32_t size() const{
                return orders_.size();
            }
        
        private:
            std::unordered_map<uint64_t, int64_t> orders_;
            std::multiset<int64_t> book_;
    };

    class OrderBook{
        public:
            OrderBook() = default;
            ~OrderBook() = default;

            void add_order(uint64_t id, int side, int64_t price, int64_t quantity){
                price = (-1 + 2 * side) * price; 
                if (cold_orders_[side].empty()){ // predictable branch, will not go into this branch after initialization
                    if (books_[side].size() < HOT_SIZE) { // predicatable, will be triggered 256 times on initialization
                        hot_sides_[id] = side;
                        books_[side].add_order(id, price);
                    }
                    else {
                        cold_orders_[side][id] = price; // will be triggered once on initialization
                    }
                    return;
                }

                if (price < books_[side].worst() && books_[side].size() < HOT_SIZE){ // push the better price into hot store
                    hot_sides_[id] = side;
                    books_[side].add_order(id, price);
                }
                else {
                    cold_orders_[side][id] = price;
                }
            }

            void cancel_order(uint64_t id){
                auto it = hot_sides_.find(id);
                if (it != hot_sides_.end()){
                    bool side = it->second;
                    books_[side].cancel_order(id);
                    hot_sides_.erase(id);

                    if (books_[side].size() == 0){
                        pull_from_cold(side);
                    }

                    return;
                }

                cold_orders_[0].erase(id);
                cold_orders_[1].erase(id);
            }

            int64_t best_bid() const {
                return - books_[0].best();
            }

            int64_t best_ask() const{
                return books_[1].best();
            }

        private: 
            std::unordered_map<uint64_t, int64_t> cold_orders_[2]; // l3 cache  
            std::unordered_map<uint64_t, bool> hot_sides_; // l1 cache
            SmallBook books_[2]; // l1 cache , maybe spill a little bit into l2

            void pull_from_cold(int side){
                 // max heap, top of heap will be the worst price
                 // top K algorithm
                std::cout << "triggering rebuild" << std::endl;
                std::priority_queue<Order, std::vector<Order>, Ascending> rebuild_buffer_; 
               
                for (auto it = cold_orders_[side].begin(); it != cold_orders_[side].end(); it++){
                    rebuild_buffer_.push({it->second, it->first});
                    if (rebuild_buffer_.size() > HOT_SIZE){
                        rebuild_buffer_.pop();
                    }
                }

                while (!rebuild_buffer_.empty()){
                    Order order = rebuild_buffer_.top();
                    books_[side].add_order(order.id, order.price);
                    cold_orders_[side].erase(order.id);
                    rebuild_buffer_.pop();
                }
            }
    };
} 
