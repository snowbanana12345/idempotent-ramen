#include "../base.h"

namespace hftu{

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

            Order pop_worst(){
                if (book_.empty()) return {INT64_MAX, 0};
                auto worst_it = book_.rbegin();
                int64_t worst_price = *worst_it;
                auto it = std::find_if(orders_.begin(), orders_.end(), [worst_price](const auto& entry) { return entry.second == worst_price;});
                if (it == orders_.end()){
                    std::cout << "Order not found error" << std::endl;
                    return {INT64_MAX, 0};
                }
                Order removed = {it->second, it->first};
                orders_.erase(it);
                book_.erase(std::next(worst_it).base());
                return removed;
            }

            uint32_t size() const{
                return orders_.size();
            }
        
        private:
            std::unordered_map<uint64_t, int64_t> orders_;
            std::multiset<int64_t> book_;
    };
}