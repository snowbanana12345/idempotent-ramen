#include "solution.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace hftu;
using namespace testing;

class VenueMock : public Venue {
public:
    MOCK_METHOD(uint64_t, send_order, 
                (uint64_t our_id, uint16_t symbol, int side, 
                 int64_t price, int64_t qty), 
                (override));

    MOCK_METHOD(uint64_t, modify_order,
                (uint64_t exchange_id, int64_t new_price, int64_t new_qty),
                (override));

    MOCK_METHOD(void, cancel_order,
                (uint64_t exchange_id),
                (override));

    ~VenueMock() override = default;
};

TEST(AddOrder, Bid){
    uint64_t our_id = 12;
    uint64_t exchange_id = 16;
    int side = 0;
    uint16_t symbol = 2;
    uint64_t price = 1001;
    uint64_t quantity = 47;
    VenueMock venue;
    MultiOrderBook book(venue);
    
    EXPECT_CALL(venue, send_order(_,_,_,_,_)).WillOnce(Return(exchange_id));
    book.send_order(our_id, symbol, side, price, quantity);

    QueuePosition q = book.get_queue_position(our_id);
    EXPECT_EQ(q.index, -1);

    book.add_order(exchange_id, symbol, side, price, quantity);

    TopLevel bid = book.best_bid(symbol);
    EXPECT_EQ(bid.count, 1);
    EXPECT_EQ(bid.qty, quantity);
    EXPECT_EQ(bid.price, price);

    q = book.get_queue_position(our_id);
    EXPECT_EQ(q.index, 0);
    EXPECT_EQ(q.qty_ahead, 0);
}

TEST(AddOrder, Ask){
    uint64_t our_id = 12;
    uint64_t exchange_id = 16;
    int side = 1;
    uint16_t symbol = 2;
    uint64_t price = 1001;
    uint64_t quantity = 47;
    VenueMock venue;
    MultiOrderBook book(venue);

    EXPECT_CALL(venue, send_order(_,_,_,_,_)).WillOnce(Return(exchange_id));
    book.send_order(our_id, symbol, side, price, quantity);
    book.add_order(exchange_id, symbol, side, price, quantity);

    TopLevel ask = book.best_ask(symbol);
    EXPECT_EQ(ask.count, 1);
    EXPECT_EQ(ask.qty, quantity);
    EXPECT_EQ(ask.price, price);
}

TEST(Modify, QuantityDecrease){
    // a quantity decrease will not change an order's queue position
    // modify order 1
    // assert that the queue position does not change
    uint64_t our_id_1 = 12;
    uint64_t our_id_2 = 13;
    uint64_t exch_id_1 = 16;
    uint64_t exch_id_2 = 19;
    int side = 0;
    uint16_t symbol = 2;
    uint64_t price = 1001;
    uint64_t qty_1 = 47;
    uint64_t qty_2 = 35;
    uint64_t qty_3 = 27;
    VenueMock venue;
    MultiOrderBook book(venue);
    
    EXPECT_CALL(venue, send_order(_,_,_,_,_)).WillOnce(Return(exch_id_1));
    book.send_order(our_id_1, symbol, side, price, qty_1);
    book.add_order(exch_id_1, symbol, side, price, qty_1);
    EXPECT_CALL(venue, send_order(_,_,_,_,_)).WillOnce(Return(exch_id_2));
    book.send_order(our_id_2, symbol, side, price, qty_2);
    book.add_order(exch_id_2, symbol, side, price, qty_2);

    TopLevel bid = book.best_bid(symbol);
    EXPECT_EQ(bid.price, price);
    EXPECT_EQ(bid.qty, qty_1 + qty_2);

    QueuePosition q1 = book.get_queue_position(our_id_1);
    EXPECT_EQ(q1.index, 0);
    EXPECT_EQ(q1.qty_ahead, 0);
    QueuePosition q2 = book.get_queue_position(our_id_2);
    EXPECT_EQ(q2.index, 1);
    EXPECT_EQ(q2.qty_ahead, qty_1);

    EXPECT_CALL(venue, modify_order(_,_,_)).WillOnce(Return(exch_id_1));
    book.modify_our_order(our_id_1, price, qty_3);
    book.modify_order(exch_id_1, qty_3);

    bid = book.best_bid(symbol);
    EXPECT_EQ(bid.price, price);
    EXPECT_EQ(bid.qty, qty_2 + qty_3);

    q1 = book.get_queue_position(our_id_1);
    EXPECT_EQ(q1.index, 0);
    EXPECT_EQ(q1.qty_ahead, 0);
    q2 = book.get_queue_position(our_id_2);
    EXPECT_EQ(q2.index, 1);
    EXPECT_EQ(q2.qty_ahead, qty_3);
}

TEST(Modify, QuantityIncrease){
    // a quantity decrease will not change an order's queue position
    // modify order 1
    // assert that the queue position does not change
    uint64_t our_id_1 = 12;
    uint64_t our_id_2 = 13;
    uint64_t exch_id_1 = 16;
    uint64_t exch_id_2 = 19;
    uint64_t exch_id_3 = 25;
    int side = 1;
    uint16_t symbol = 2;
    uint64_t price = 1001;
    uint64_t qty_1 = 47;
    uint64_t qty_2 = 35;
    uint64_t qty_3 = 57;
    VenueMock venue;
    MultiOrderBook book(venue);
    
    EXPECT_CALL(venue, send_order(_,_,_,_,_)).WillOnce(Return(exch_id_1));
    book.send_order(our_id_1, symbol, side, price, qty_1);
    book.add_order(exch_id_1, symbol, side, price, qty_1);
    EXPECT_CALL(venue, send_order(_,_,_,_,_)).WillOnce(Return(exch_id_2));
    book.send_order(our_id_2, symbol, side, price, qty_2);
    book.add_order(exch_id_2, symbol, side, price, qty_2);

    TopLevel ask = book.best_ask(symbol);
    EXPECT_EQ(ask.price, price);
    EXPECT_EQ(ask.qty, qty_1 + qty_2);

    QueuePosition q1 = book.get_queue_position(our_id_1);
    EXPECT_EQ(q1.index, 0);
    EXPECT_EQ(q1.qty_ahead, 0);
    QueuePosition q2 = book.get_queue_position(our_id_2);
    EXPECT_EQ(q2.index, 1);
    EXPECT_EQ(q2.qty_ahead, qty_1);

    EXPECT_CALL(venue, modify_order(_,_,_)).WillOnce(Return(exch_id_3));
    book.modify_our_order(our_id_1, price, qty_3);
    book.cancel_order(exch_id_1);
    book.add_order(exch_id_3, symbol, side, price, qty_3);

    ask = book.best_ask(symbol);
    EXPECT_EQ(ask.price, price);
    EXPECT_EQ(ask.qty, qty_3 + qty_2);

    // the queue position swaps over since the increase in qty forces order1 to give up its queue position
    q1 = book.get_queue_position(our_id_1);
    EXPECT_EQ(q1.index, 1);
    EXPECT_EQ(q1.qty_ahead, qty_2);
    q2 = book.get_queue_position(our_id_2);
    EXPECT_EQ(q2.index, 0);
    EXPECT_EQ(q2.qty_ahead, 0);
}

TEST(Modify, PriceChange){
    // a quantity decrease will not change an order's queue position
    // modify order 1
    // assert that the queue position does not change
    uint64_t our_id_1 = 12;
    uint64_t our_id_2 = 13;
    uint64_t exch_id_1 = 16;
    uint64_t exch_id_2 = 19;
    uint64_t exch_id_3 = 25;
    int side = 1;
    uint16_t symbol = 2;
    uint64_t price_1 = 1001;
    uint64_t price_2 = 1000;
    uint64_t price_3 = 999;
    uint64_t qty_1 = 47;
    uint64_t qty_2 = 35;
    VenueMock venue;
    MultiOrderBook book(venue);
    
    EXPECT_CALL(venue, send_order(_,_,_,_,_)).WillOnce(Return(exch_id_1));
    book.send_order(our_id_1, symbol, side, price_1, qty_1);
    book.add_order(exch_id_1, symbol, side, price_1, qty_1);
    EXPECT_CALL(venue, send_order(_,_,_,_,_)).WillOnce(Return(exch_id_2));
    book.send_order(our_id_2, symbol, side, price_2, qty_2);
    book.add_order(exch_id_2, symbol, side, price_2, qty_2);

    TopLevel ask = book.best_ask(symbol);
    EXPECT_EQ(ask.price, price_2);
    EXPECT_EQ(ask.qty, qty_2);

    QueuePosition q1 = book.get_queue_position(our_id_1);
    EXPECT_EQ(q1.index, 1);
    EXPECT_EQ(q1.qty_ahead, qty_2);
    QueuePosition q2 = book.get_queue_position(our_id_2);
    EXPECT_EQ(q2.index, 0);
    EXPECT_EQ(q2.qty_ahead, 0);

    EXPECT_CALL(venue, modify_order(_,_,_)).WillOnce(Return(exch_id_3));
    book.modify_our_order(our_id_1, price_3, qty_1);
    book.cancel_order(exch_id_1);
    book.add_order(exch_id_3, symbol, side, price_3, qty_1);

    ask = book.best_ask(symbol);
    EXPECT_EQ(ask.price, price_3);
    EXPECT_EQ(ask.qty, qty_1);

    // the queue position swaps over since the increase in qty forces order1 to give up its queue position
    q1 = book.get_queue_position(our_id_1);
    EXPECT_EQ(q1.index, 0);
    EXPECT_EQ(q1.qty_ahead, 0);
    q2 = book.get_queue_position(our_id_2);
    EXPECT_EQ(q2.index, 1);
    EXPECT_EQ(q2.qty_ahead, qty_1);
}

TEST(Query, AskSorted){
    uint64_t our_id_1 = 12; uint64_t our_id_2 = 13; uint64_t our_id_3 = 14; uint64_t our_id_4 = 15;
    uint64_t exch_id_1 = 16; uint64_t exch_id_2 = 19; uint64_t exch_id_3 = 25; uint64_t exch_id_4 = 67;
    int side = 1; uint16_t symbol = 2;

    VenueMock venue;
    MultiOrderBook book(venue);
    
    EXPECT_CALL(venue, send_order(_,_,_,_,_)).WillOnce(Return(exch_id_1));
    book.send_order(our_id_1, symbol, side, 99, 5);
    book.add_order(exch_id_1, symbol, side, 99, 5);
    EXPECT_CALL(venue, send_order(_,_,_,_,_)).WillOnce(Return(exch_id_2));
    book.send_order(our_id_2, symbol, side, 98, 7);
    book.add_order(exch_id_2, symbol, side, 98, 7);
    EXPECT_CALL(venue, send_order(_,_,_,_,_)).WillOnce(Return(exch_id_3));
    book.send_order(our_id_3, symbol, side, 99, 11);
    book.add_order(exch_id_3, symbol, side, 99, 11);
    EXPECT_CALL(venue, send_order(_,_,_,_,_)).WillOnce(Return(exch_id_4));
    book.send_order(our_id_4, symbol, side, 98, 8);
    book.add_order(exch_id_4, symbol, side, 98, 8);

    TopLevel best_ask = book.best_ask(symbol);
    EXPECT_EQ(best_ask.price, 98);
    EXPECT_EQ(best_ask.qty, 15);

    TopLevel levels[2];
    EXPECT_EQ(book.get_top_levels(symbol, side, 2, levels), 2);

    EXPECT_EQ(levels[0].price, 98);
    EXPECT_EQ(levels[0].count, 2);
    EXPECT_EQ(levels[0].qty, 15);
    EXPECT_EQ(levels[1].price, 99);
    EXPECT_EQ(levels[1].count, 2);
    EXPECT_EQ(levels[1].qty, 18);

}