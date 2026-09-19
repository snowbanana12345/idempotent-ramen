#include "base.h"

namespace hftu{


class ColdBook{
        public:
            ColdBook(){
                add_log_[0].reserve(2'000'000);
                add_log_[1].reserve(2'000'000);

                cancel_log_[0].reserve(2'000'000);
                cancel_log_[1].reserve(2'000'000);

                cold_orders_[0].reserve(1'000'000);
                cold_orders_[1].reserve(1'000'000);
            }
            ~ColdBook() = default;

            void add_order(uint64_t id, int64_t price, int side){
                add_log_[side].emplace_back(id, price);
            }

            void cancel_order(uint64_t id){
                cancel_log_[0].push_back(id);
                cancel_log_[1].push_back(id);
            }

            std::vector<Order> pull(int side, size_t K){
                // this is where we pull the top K orders from the cold store;
                // ideally, this triggers only once per 1 million operations as this function is slow as balls
                // probably on the order of >10k cycles
                std::cout << "triggering rebuild" << std::endl;

                // ---- perform compaction ----

                for (const Order& order : add_log_[side]){
                    cold_orders_[side].emplace(order.id, order.price);
                }

                for (uint64_t cancel_id : cancel_log_[side]){
                    cold_orders_[side].erase(cancel_id);
                }


                // ---- collect top orders ------

                std::priority_queue<Order, std::vector<Order>, Ascending> rebuild_buffer_; 
               
                for (auto it = cold_orders_[side].begin(); it != cold_orders_[side].end(); it++){
                    rebuild_buffer_.push({it->second, it->first});
                    if (rebuild_buffer_.size() > K){
                        rebuild_buffer_.pop();
                    }
                }

                std::vector<Order> result;

                while (!rebuild_buffer_.empty()){
                    Order order = rebuild_buffer_.top();
                    rebuild_buffer_.pop();
                    cold_orders_[side].erase(order.id);
                    result.push_back(order);
                }

                return result;
            }

            uint32_t size(int side){
                return cold_orders_[side].size();
            }

            bool empty(int side){
                return cold_orders_[side].empty();
            }
        
        private:
            std::vector<Order> add_log_[2];
            std::vector<uint64_t> cancel_log_[2];
            std::unordered_map<uint64_t, int64_t> cold_orders_[2];
    };
}