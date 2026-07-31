#include <list>
#include <map>
#include "../venue.h"
#include <iostream>

namespace hftu{
    class QueueOrder{
        public:
            void append(int64_t price, uint64_t order_id, int64_t quantity){
                queues[price].push_back({order_id, quantity});
            }

            void modify_quantity(int64_t price, uint64_t order_id, int64_t new_quantity){
                auto it = queues.find(price);
                if (it == queues.end()) return;

                for (auto qit = it->second.begin(); qit != it->second.end(); qit++){
                    if (qit->id == order_id){
                        qit->quantity = new_quantity;
                        break;
                    }
                }
            }

            void remove(int64_t price, uint64_t order_id){
                auto it = queues.find(price);
                if (it == queues.end()) return;

                auto qit = it->second.begin();
                for (; qit != it->second.end(); qit++){
                    if (qit->id == order_id){
                        break;
                    }
                }
                
                it->second.erase(qit);

                if (it->second.size() == 0){
                    queues.erase(it);
                }
            }

            QueuePosition query(int64_t price, uint64_t order_id) const{
                auto it = queues.find(price);
                if (it == queues.end()) return {-1, 0};

                int64_t qty_ahead = 0;
                int32_t index = 0;
                bool found = false;

                for (auto qit = it->second.begin(); qit != it->second.end(); qit++){
                    if (qit->id == order_id){
                        found = true;
                        break;
                    }
                    index++;
                    qty_ahead += qit->quantity;
                }
                if (found) return {index, qty_ahead};
                return {-1, 0};
            }
        
        private:
            struct Entry{
                uint64_t id;
                int64_t quantity;
            };

            std::unordered_map<int64_t, std::list<Entry>> queues;
    };
}