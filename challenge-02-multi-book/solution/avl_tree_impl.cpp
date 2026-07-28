#include "solution.h"


namespace hftu{

    class Impl{
        public: 
            void send_order(uint64_t our_id, uint16_t symbol, int side, int64_t price, int64_t qty, Venue &venue);
            void modify_our_order(uint64_t our_id, int64_t new_price, int64_t new_qty, Venue &venue);
            void cancel_our_order(uint64_t our_id, Venue &venue);

            void add_order(uint64_t exchange_id, uint16_t symbol, int side, int64_t price, int64_t qty);
            void modify_order(uint64_t exchange_id, int64_t new_qty);
            void cancel_order(uint64_t exchange_id);

            TopLevel best_bid(uint16_t symbol) const;
            TopLevel best_ask(uint16_t symbol) const;
            int get_top_levels(uint16_t symbol, int side, int n, TopLevel* out) const;
            int64_t volume_near_best(uint16_t symbol, int side, int64_t depth) const;
            QueuePosition get_queue_position(uint64_t our_id) const;

        private:
            struct Order {
                uint16_t symbol;
                int8_t side;
                int64_t price;
                int64_t qty;
            };

            std::unordered_map<uint64_t, Order> orders_;        // exchange_id -> order
            std::unordered_map<uint64_t, uint64_t> our_orders_; // our_id -> exchange_id
    };
}

#include "pimpl.h"