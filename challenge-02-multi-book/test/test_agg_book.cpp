#include "agg_book.h"
#include <gmock/gmock.h>

using namespace hftu;

TEST(Add, One){
    AggBook<true> book;
    book.add(5, 7);

    TopLevel bid = book.best();
    EXPECT_EQ(bid.count, 1);
    EXPECT_EQ(bid.qty, 7);
    EXPECT_EQ(bid.price, 5);
}

TEST(AggBook, remove){
    AggBook<true> book;
    book.add(5, 7);

    TopLevel bid = book.best();
    EXPECT_EQ(bid.count, 1);
    EXPECT_EQ(bid.qty, 7);
    EXPECT_EQ(bid.price, 5);

    book.remove(5, 7);
    bid = book.best();
    EXPECT_EQ(bid.count, 0);
    EXPECT_EQ(bid.qty, 0);
    EXPECT_EQ(bid.price, 0);

}