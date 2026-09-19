#include "../base.h"
#include <boost/container/static_vector.hpp>

namespace hftu{


class SmallBook{
        public:
            SmallBook() = default;
            ~SmallBook() = default;

            void add_order(uint64_t id, int64_t price){
                int idx = 0;
                for (int i = 0; i < orders_.size(); i++){
                    idx += (price > orders_[i].price);
                }

                orders_.insert(orders_.begin() + idx, {price, id});
            }

            void cancel_order(uint64_t id){
                for (int i = 0; i < orders_.size(); i++){
                    if (orders_[i].id == id) {
                        orders_.erase(orders_.begin() + i);
                        return;
                    }
                }
            }

            int64_t best() const {
                if (orders_.empty()) return 0;
                return orders_.cbegin()->price;
            }

            int64_t worst() const {
                if (orders_.empty()) return 0;
                return orders_.crbegin()->price;
            }

            Order pop_worst(){
                if (orders_.empty()) return {INT64_MAX, 0};
                Order removed = *orders_.rbegin();
                orders_.pop_back();
                return removed;
            }

            uint32_t size() const{
                return orders_.size();
            }
        
        private:
            boost::container::static_vector<Order, 32> orders_;
    };
}