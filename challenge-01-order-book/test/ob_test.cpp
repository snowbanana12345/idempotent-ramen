#include "solution.h"
#include <gtest/gtest.h>

using namespace hftu;

TEST(AddOrder, Bid){
    OrderBook ob;
    ob.add_order(7, 0, 15, 2);
    EXPECT_EQ(ob.best_bid(), 15);
    ob.add_order(8, 0, 17, 3);
    EXPECT_EQ(ob.best_bid(), 17);
    ob.add_order(9, 0, 14, 3);
    ob.add_order(10, 0, 18, 3);
    EXPECT_EQ(ob.best_bid(), 18);
}

TEST(AddOrder, Ask){
    OrderBook ob;
    ob.add_order(4, 1, 15, 2);
    EXPECT_EQ(ob.best_ask(), 15);
    ob.add_order(9, 1, 17, 3);
    EXPECT_EQ(ob.best_ask(), 15);
    ob.add_order(15, 1, 14, 3);
    ob.add_order(17, 1, 18, 3);
    EXPECT_EQ(ob.best_ask(), 14);
}

TEST(BEST, EMPTY){
    OrderBook ob;
    EXPECT_EQ(ob.best_ask(), 0);
    EXPECT_EQ(ob.best_bid(), 0);
}

TEST(CANCEL, BID){
    OrderBook ob;
    ob.add_order(7, 0, 15, 2);
    EXPECT_EQ(ob.best_bid(), 15);
    ob.cancel_order(7);
    EXPECT_EQ(ob.best_bid(), 0);
}

TEST(CANCEL, ASK){
    OrderBook ob;
    ob.add_order(7, 1, 15, 2);
    EXPECT_EQ(ob.best_ask(), 15);
    ob.cancel_order(7);
    EXPECT_EQ(ob.best_ask(), 0);
}


TEST(CANCEL, SORT_BID){
    OrderBook ob;
    ob.add_order(7, 0, 15, 2);
    ob.add_order(8, 0, 17, 2);
    ob.add_order(9, 0, 16, 2);
    ob.add_order(10, 0, 18, 2);
    EXPECT_EQ(ob.best_bid(), 18);
    ob.cancel_order(9);
    EXPECT_EQ(ob.best_bid(), 18);
    ob.cancel_order(10);
    EXPECT_EQ(ob.best_bid(), 17);
    ob.cancel_order(8);
    EXPECT_EQ(ob.best_bid(), 15);
}

TEST(CANCEL, SORT_ASK){
    OrderBook ob;
    ob.add_order(7, 1, 15, 2);
    ob.add_order(8, 1, 17, 2);
    ob.add_order(9, 1, 16, 2);
    ob.add_order(10, 1, 18, 2);
    EXPECT_EQ(ob.best_ask(), 15);
    ob.cancel_order(9);
    EXPECT_EQ(ob.best_ask(), 15);
    ob.cancel_order(10);
    EXPECT_EQ(ob.best_ask(), 15);
    ob.cancel_order(7);
    EXPECT_EQ(ob.best_ask(), 17);
}

TEST(DUPLICATE, BID){
    OrderBook ob;
    ob.add_order(7, 0, 15, 2);
    ob.add_order(8, 0, 16, 2);
    ob.add_order(9, 0, 16, 2);
    EXPECT_EQ(ob.best_bid(), 16);
    ob.cancel_order(8);
    EXPECT_EQ(ob.best_bid(), 16);
    ob.cancel_order(9);
    EXPECT_EQ(ob.best_bid(), 15);
}

