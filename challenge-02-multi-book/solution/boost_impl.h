#include "base.h"
#include "boost_book.h"

namespace hftu {

class MultiOrderBook {
    public:
        explicit MultiOrderBook(Venue& venue) : venue_(venue){
            our_orders_.reserve(1'000'000);
            orders_.reserve(1'000'000);
        }
        ~MultiOrderBook() = default;

        void send_order(uint64_t our_id, uint16_t symbol, int side, int64_t price, int64_t qty) {
            uint64_t exchange_id = venue_.send_order(our_id, symbol, side, price, qty);
            our_orders_[our_id] = exchange_id;
        }

        void modify_our_order(uint64_t our_id, int64_t new_price, int64_t new_qty) {
            auto it = our_orders_.find(our_id);
            if (it == our_orders_.end()) return;
            uint64_t new_eid = venue_.modify_order(it->second, new_price, new_qty);
            it->second = new_eid;
        }

        void cancel_our_order(uint64_t our_id) {
            auto it = our_orders_.find(our_id);
            if (it == our_orders_.end()) return;
            venue_.cancel_order(it->second);
            our_orders_.erase(it);
        }
        
        // === Exchange feed ===
        // All orders (everyone's, including ours when they appear).
        void add_order(uint64_t exchange_id, uint16_t symbol, int side, int64_t price, int64_t qty) {
            
            orders_[exchange_id] = {symbol, static_cast<int8_t>(side), price, qty};

            SymbolBook& book = books_[side * NUM_SYMBOLS + symbol];
            return book.add_order(exchange_id, price, qty);
        }

        // Qty-down only — order keeps its queue position.
        void modify_order(uint64_t exchange_id, int64_t new_qty) {
            auto it = orders_.find(exchange_id);
            if (it == orders_.end()) return;
            auto& order = it->second;

            SymbolBook& book = books_[order.side * NUM_SYMBOLS + order.symbol];
            return book.modify_order(exchange_id, new_qty);
        }

        // Remove order from book.
        void cancel_order(uint64_t exchange_id) {
            auto it = orders_.find(exchange_id);
            if (it == orders_.end()) return;
            auto& order = it->second;

            SymbolBook& book = books_[order.side * NUM_SYMBOLS + order.symbol];
            return book.cancel_order(exchange_id);
        }

        // === Queries ===
        TopLevel best_bid(uint16_t symbol) const {
            SymbolBook& book = books_[side * NUM_SYMBOLS + symbol];
            return book.best_bid();
        }

        TopLevel best_ask(uint16_t symbol) const {
            SymbolBook& book = books_[side * NUM_SYMBOLS + symbol];
            return book.best_ask();
        }

        int get_top_levels(uint16_t symbol, int side, int depth, TopLevel* out) const {
            SymbolBook& book = books_[side * NUM_SYMBOLS + symbol];
            return book.get_top_levels(depth, out);
        }

        int64_t volume_near_best(uint16_t symbol, int side, int64_t depth) const {
            SymbolBook& book = books_[side * NUM_SYMBOLS + symbol];
            return book.volume_near_best(depth);
        }

        QueuePosition get_queue_position(uint64_t our_id) const {
            auto oit = our_orders_.find(our_id);
            if (oit == our_orders_.end()) {
                return {-1, 0};
            }

            uint64_t exchange_id = oit->second;
            auto eit = orders_.find(exchange_id);
            if (eit == orders_.end()) {
                return {-1, 0}; 
            }

            return books_[eit->second.side * NUM_SYMBOLS + eit->second.symbol].get_queue_position(exchange_id);
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
        SymbolBook books_[2 * NUM_SYMBOLS];

        Venue& venue_;
    };
}
