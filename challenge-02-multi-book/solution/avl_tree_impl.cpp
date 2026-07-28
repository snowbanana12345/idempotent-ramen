#include "solution.h"
#include <array>


namespace hftu{
    template <bool ascending>
    class AggBook{
        public:
            void add(int64_t price, int64_t quantity){
                auto it = book.find(price);
                if (it == book.end()){
                    book[price] = {1, quantity};
                }
                else {
                    book[price].count++;
                    book[price].quantity += quantity;
                }
            }

            void modify(int64_t price, int64_t change_quantity){
                book[price].quantity += change_quantity;
            }

            void remove(int64_t price, int64_t quantity){
                book[price].count--;
                book[price].quantity -= quantity;
                if (book[price].count == 0){
                    book.erase(price);
                }
            }

            TopLevel best() const{
                if (book.empty()) return {0, 0, 0};
                if (ascending){
                    auto it = book.begin();
                    return {it->first, it->second.quantity, it->second.count};
                }
                else {
                    auto it = book.rbegin();
                    return {it->first, it->second.quantity, it->second.count};
                }
            }

            int get_top_levels(int n, TopLevel* out) const {
                if (ascending){
                    auto it = book.begin();
                    int i = 0;
                    while(i < n && it != book.end()){
                        out[i].price = it->first;
                        out[i].qty = it->second.quantity;
                        out[i].count = it->second.count;
                        it++; i++;
                    }
                    return i;
                }
                else {
                    auto it = book.rbegin();
                    int i = 0;
                    while(i < n && it != book.rend()){
                        out[i].price = it->first;
                        out[i].qty = it->second.quantity;
                        out[i].count = it->second.count;
                        it++; i++;
                    }
                    return i;
                }
            }

            int64_t volume_near_best(int64_t depth) const {
                if (depth == 0 || book.empty()) return 0;
                if (ascending){
                    auto it = book.begin();
                    int64_t best_price = it->first;
                    int64_t volume = 0;
                    for (; it != book.end() && it->first <= best_price + depth - 1; it++){
                        volume += it->second.quantity;
                    }
                    return volume;
                }
                else{
                    auto it = book.rbegin();
                    int64_t best_price = it->first;
                    int64_t volume = 0;
                    for (; it != book.rend() && it->first >= best_price - depth + 1; it++){
                        volume += it->second.quantity;
                    }
                    return volume;
                }
            }

        private:
            struct Level{
                int32_t count;
                int64_t quantity;
            };
            std::map<int64_t, Level> book;
    };

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

                if (side){ // bid
                    bid_books[symbol].add(price, qty);
                }
                else {
                    ask_books[symbol].add(price, qty);
                }
            }

            void modify_order(uint64_t exchange_id, int64_t new_qty){
                auto it = orders_.find(exchange_id);
                if (it == orders_.end()) return;
                auto& order = it->second;
                int64_t old_qty = order.qty;
                order.qty = new_qty;
                const auto& [symbol, side, price, qty] = order;

                if (side){
                    bid_books[symbol].modify(price, new_qty - old_qty);
                }
                else {
                    ask_books[symbol].modify(price, new_qty - old_qty);
                }
            }

            void cancel_order(uint64_t exchange_id){
                auto it = orders_.find(exchange_id);
                if (it == orders_.end()) return;
                const auto& [symbol, side, price, qty] = it->second;
                orders_.erase(it);

                if (side){ // bid
                    bid_books[symbol].remove(price, qty);
                }
                else{
                    ask_books[symbol].remove(price, qty);
                }
            }

            TopLevel best_bid(uint16_t symbol) const{
                return bid_books[symbol].best();
            }

            TopLevel best_ask(uint16_t symbol) const{
                return ask_books[symbol].best();
            }

            int get_top_levels(uint16_t symbol, int side, int n, TopLevel* out) const{
                if (side){
                    return bid_books[symbol].get_top_levels(n, out);
                }
                else {
                    return ask_books[symbol].get_top_levels(n, out);
                }
            }

            int64_t volume_near_best(uint16_t symbol, int side, int64_t depth) const{
                if (side){
                    return bid_books[symbol].volume_near_best(depth);
                }
            }   

            QueuePosition get_queue_position(uint64_t our_id) const{
                return {-1, 0};
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
    };
}

#include "pimpl.h"