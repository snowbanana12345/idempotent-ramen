#include "solution.h"
#include <array>
#include "agg_book.h"
#include "queue_order.h"


namespace hftu{
    class Impl{
        public: 
            void send_order(uint64_t our_id, uint16_t symbol, int side, int64_t price, int64_t qty, Venue& venue) {
                uint64_t exchange_id = venue.send_order(our_id, symbol, side, price, qty);
                our_orders_[our_id] = exchange_id;
            }

            void modify_our_order(uint64_t our_id, int64_t new_price, int64_t new_qty, Venue& venue) {
                auto it = our_orders_.find(our_id);
                if (it == our_orders_.end()) return;
                uint64_t new_eid = venue.modify_order(it->second, new_price, new_qty);
                it->second = new_eid;
            }

            void cancel_our_order(uint64_t our_id, Venue& venue) {
                auto it = our_orders_.find(our_id);
                if (it == our_orders_.end()) return;
                venue.cancel_order(it->second);
                our_orders_.erase(it);
            }

            void add_order(uint64_t exchange_id, uint16_t symbol, int side, int64_t price, int64_t qty){
                orders_[exchange_id] = {symbol, static_cast<int8_t>(side), price, qty};
                queue_orders[symbol].append(price, exchange_id, qty);

                if (side){ // side = 1 means ask
                    ask_books[symbol].add(price, qty);
                }
                else {
                    bid_books[symbol].add(price, qty);
                }
            }

            void modify_order(uint64_t exchange_id, int64_t new_qty){
                auto it = orders_.find(exchange_id);
                if (it == orders_.end()) return;
                auto& order = it->second;
                int64_t old_qty = order.qty;
                order.qty = new_qty;
                const auto& [symbol, side, price, qty] = order;
                queue_orders[symbol].modify_quantity(price, exchange_id, new_qty);

                if (side){ // side = 1 means ask
                    ask_books[symbol].modify(price, new_qty - old_qty);
                }
                else {
                    bid_books[symbol].modify(price, new_qty - old_qty);
                }
            }

            void cancel_order(uint64_t exchange_id){
                auto it = orders_.find(exchange_id);
                if (it == orders_.end()) return;
                const auto& [symbol, side, price, qty] = it->second;
                queue_orders[symbol].remove(price, exchange_id);

                if (side){ // side = 1 means ask
                    ask_books[symbol].remove(price, qty);
                }
                else{
                    bid_books[symbol].remove(price, qty);
                }
                orders_.erase(it);
            }

            TopLevel best_bid(uint16_t symbol) const{
                return bid_books[symbol].best();
            }

            TopLevel best_ask(uint16_t symbol) const{
                return ask_books[symbol].best();
            }

            int get_top_levels(uint16_t symbol, int side, int n, TopLevel* out) const{
                if (side){
                    return ask_books[symbol].get_top_levels(n, out);
                }
                else {
                    return bid_books[symbol].get_top_levels(n, out);
                }
            }

            int64_t volume_near_best(uint16_t symbol, int side, int64_t depth) const{
                if (side){
                    return ask_books[symbol].volume_near_best(depth);
                }
                else{
                    return bid_books[symbol].volume_near_best(depth);   
                }
            }   

            QueuePosition get_queue_position(uint64_t our_id) const{
                auto oit = our_orders_.find(our_id);
                if (oit == our_orders_.end()) {
                    return {-1, 0};
                }

                uint64_t exchange_id = oit->second;
                auto eit = orders_.find(exchange_id);
                if (eit == orders_.end()) {
                    return {-1, 0}; 
                }

                const auto& [symbol, side, price, qty] = eit->second;
                return queue_orders[symbol].query(price, exchange_id);
            }

        private:
            struct Order {
                uint16_t symbol;
                int8_t side;
                int64_t price;
                int64_t qty;
            };

            std::unordered_map<uint64_t, Order> orders_;        // exchange_id -> order
            std::unordered_map<uint64_t, uint64_t> our_orders_; // our_id -> exchange_id

            AggBook<true> ask_books[200];
            AggBook<false> bid_books[200];
            QueueOrder queue_orders[200];
    };
}

#include "pimpl.h"