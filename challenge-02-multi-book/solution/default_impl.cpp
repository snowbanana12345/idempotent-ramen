// Challenge 02: Multi-Symbol Order Book — Naive Reference Implementation
// This is a correct but slow reference. You can do much better!

#include "solution.h"

namespace hftu {
    class Impl {
    public:
        Impl() {
            our_orders_.reserve(1'000'000);
            orders_.reserve(1'000'000);
        }

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

        // === Exchange feed ===
        // All orders (everyone's, including ours when they appear).
        void add_order(uint64_t exchange_id, uint16_t symbol, int side,
                              int64_t price, int64_t qty) {
            orders_[exchange_id] = {symbol, static_cast<int8_t>(side), price, qty};
            auto& levels = (side == 0) ? books_[symbol].bids : books_[symbol].asks;
            auto& level = levels[price];
            level.queue.push_back({exchange_id, qty});
            level.total_qty += qty;
            level.count++;
        }

        // Qty-down only — order keeps its queue position.
        void modify_order(uint64_t exchange_id, int64_t new_qty) {
            auto it = orders_.find(exchange_id);
            if (it == orders_.end()) return;
            auto& order = it->second;
            int64_t old_qty = order.qty;
            order.qty = new_qty;

            auto& levels = (order.side == 0) ? books_[order.symbol].bids : books_[order.symbol].asks;
            auto lit = levels.find(order.price);
            if (lit == levels.end()) return;

            lit->second.total_qty += (new_qty - old_qty);
            for (auto& [eid, qty] : lit->second.queue) {
                if (eid == exchange_id) {
                    qty = new_qty;
                    break;
                }
            }
        }

        // Remove order from book.
        void cancel_order(uint64_t exchange_id) {
            auto it = orders_.find(exchange_id);
            if (it == orders_.end()) return;
            auto& order = it->second;

            auto& levels = (order.side == 0) ? books_[order.symbol].bids : books_[order.symbol].asks;
            auto lit = levels.find(order.price);
            if (lit != levels.end()) {
                auto& level = lit->second;
                for (auto qit = level.queue.begin(); qit != level.queue.end(); ++qit) {
                    if (qit->first == exchange_id) {
                        level.total_qty -= qit->second;
                        level.count--;
                        level.queue.erase(qit);
                        break;
                    }
                }
                if (level.count == 0)
                    levels.erase(lit);
            }
            orders_.erase(it);
        }

        // === Queries ===
        TopLevel best_bid(uint16_t symbol) const {
            auto& bids = books_[symbol].bids;
            if (bids.empty()) return {};
            auto it = bids.rbegin();
            return {it->first, it->second.total_qty, it->second.count};
        }

        TopLevel best_ask(uint16_t symbol) const {
            auto& asks = books_[symbol].asks;
            if (asks.empty()) return {};
            auto it = asks.begin();
            return {it->first, it->second.total_qty, it->second.count};
        }

        // Write up to n best levels into out[]. Returns levels written.
        int get_top_levels(uint16_t symbol, int side, int n, TopLevel* out) const {
            int written = 0;
            if (side == 0) {
                auto& bids = books_[symbol].bids;
                for (auto it = bids.rbegin(); it != bids.rend() && written < n; ++it, ++written)
                    out[written] = {it->first, it->second.total_qty, it->second.count};
            } else {
                auto& asks = books_[symbol].asks;
                for (auto it = asks.begin(); it != asks.end() && written < n; ++it, ++written)
                    out[written] = {it->first, it->second.total_qty, it->second.count};
            }
            return written;
        }

        // Total qty within `depth` ticks of best price.
        int64_t volume_near_best(uint16_t symbol, int side, int64_t depth) const {
            int64_t total = 0;
            if (side == 0) {
                auto& bids = books_[symbol].bids;
                if (bids.empty()) return 0;
                int64_t best = bids.rbegin()->first;
                int64_t min_price = best - depth + 1;
                for (auto it = bids.rbegin(); it != bids.rend() && it->first >= min_price; ++it)
                    total += it->second.total_qty;
            } else {
                auto& asks = books_[symbol].asks;
                if (asks.empty()) return 0;
                int64_t best = asks.begin()->first;
                int64_t max_price = best + depth - 1;
                for (auto it = asks.begin(); it != asks.end() && it->first <= max_price; ++it)
                    total += it->second.total_qty;
            }
            return total;
        }

        // Queue position for one of our orders.
        QueuePosition get_queue_position(uint64_t our_id) const {
            auto oit = our_orders_.find(our_id);
            if (oit == our_orders_.end()) {
                return {-1, 0};
            }

            uint64_t exchange_id = oit->second;
            auto eit = orders_.find(exchange_id);
            if (eit == orders_.end()) {
                return {-1, 0}; // not in book yet
            }

            auto& order = eit->second;
            auto& levels = (order.side == 0) ? books_[order.symbol].bids : books_[order.symbol].asks;
            auto lit = levels.find(order.price);
            if (lit == levels.end()) return {-1, 0};

            int32_t index = 0;
            int64_t qty_ahead = 0;
            for (auto& [eid, qty] : lit->second.queue) {
                if (eid == exchange_id)
                    return {index, qty_ahead};
                index++;
                qty_ahead += qty;
            }
            return {-1, 0};
        }

    private:
        struct Order {
            uint16_t symbol;
            int8_t side;
            int64_t price;
            int64_t qty;
        };

        struct Level {
            int64_t total_qty = 0;
            int32_t count = 0;
            std::list<std::pair<uint64_t, int64_t>> queue; // (exchange_id, qty) in FIFO order
        };

        struct SymbolBook {
            std::map<int64_t, Level> bids; // rbegin() = best bid
            std::map<int64_t, Level> asks; // begin() = best ask
        };

        std::unordered_map<uint64_t, Order> orders_;        // exchange_id -> order
        std::unordered_map<uint64_t, uint64_t> our_orders_; // our_id -> exchange_id
        SymbolBook books_[NUM_SYMBOLS];
    };
}

#include "pimpl.h"
