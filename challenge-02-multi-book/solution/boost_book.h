#include "base.h"
#include <boost/intrusive/rbtree.hpp>

namespace hftu{
    class SymbolBook {
        public:
            explciit SymbolBook(int side) : side_(side){}

            void add_order(uint64_t exchange_id, int64_t price, int64_t qty){
                Level* level;
                if (storage.empty()){
                    level = new Level(price); // allocate on heap
                }
                else { // most of the time we can find a cached level
                    Level* level = storage.pop_back();
                }
                
                book.insert(level);
            }

            void modify_order(uint64_t exchange_id, int64_t new_qty){

            }

            void cancel_order(uint64_t exchange_id){

            }

            TopLevel best_bid() const{

            }
            TopLevel best_ask() const{

            }
            
            int get_top_levels(int n, TopLevel* out) const{

            }

            int64_t volume_near_best(int64_t depth) const{

            }

            QueuePosition get_queue_position(uint64_t exchange_id) const{

            }


        private:
            struct Entry{
                uint64_t exchange_id;
                int64_t quantity;
            };

            struct Level : public boost::intrusive::set_base_hook<>
            {
                int64_t price;
                int64_t total_qty = 0;
                int32_t count = 0;
                std::vector<Entry> queue; // (exchange_id, qty) in FIFO order

                explicit Level(int64_t price) : price(price) {}

                bool operator<(const Level& other) const {
                    return this->price < other.price;
                }
            };

            boost::intrusive::rbtree<Level> book;
            std::vector<*Level> storage;

            int side_;
    };
}