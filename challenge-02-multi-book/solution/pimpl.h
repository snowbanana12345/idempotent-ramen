 #pragma once
 
 namespace hftu{
    MultiOrderBook::MultiOrderBook(Venue& venue) : venue_(venue), impl(std::make_unique<Impl>()) {

    }

    MultiOrderBook::~MultiOrderBook() = default;

    // === Our order management ===

    void MultiOrderBook::send_order(uint64_t our_id, uint16_t symbol, int side,
                                    int64_t price, int64_t qty) {
        impl->send_order(our_id, symbol, side, price, qty, venue_);
    }

    void MultiOrderBook::modify_our_order(uint64_t our_id, int64_t new_price, int64_t new_qty) {
        impl->modify_our_order(our_id, new_price, new_qty, venue_);
    }

    void MultiOrderBook::cancel_our_order(uint64_t our_id) {
        impl->cancel_our_order(our_id, venue_);
    }

    void MultiOrderBook::add_order(uint64_t exchange_id, uint16_t symbol, int side,
                                int64_t price, int64_t qty) {
        impl->add_order(exchange_id, symbol, side, price, qty);
    }

    void MultiOrderBook::modify_order(uint64_t exchange_id, int64_t new_qty) {
        impl->modify_order(exchange_id, new_qty);
    }

    void MultiOrderBook::cancel_order(uint64_t exchange_id) {
        impl->cancel_order(exchange_id);
    }

    TopLevel MultiOrderBook::best_bid(uint16_t symbol) const {
        return impl->best_bid(symbol);
    }

    TopLevel MultiOrderBook::best_ask(uint16_t symbol) const {
        return impl->best_ask(symbol);
    }

    int MultiOrderBook::get_top_levels(uint16_t symbol, int side, int n,
                                    TopLevel* out) const {
        return impl->get_top_levels(symbol, side, n, out);
    }

    int64_t MultiOrderBook::volume_near_best(uint16_t symbol, int side, int64_t depth) const {
        return impl->volume_near_best(symbol, side, depth);
    }

    QueuePosition MultiOrderBook::get_queue_position(uint64_t our_id) const {
        return impl->get_queue_position(our_id);
    }
}