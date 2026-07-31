#include "queue_order.h"
#include <gmock/gmock.h>

using namespace hftu;

TEST(Append, One){
    QueueOrder q;
    q.append(1, 3, 1);
    QueuePosition pos = q.query(1, 3);
    EXPECT_EQ(pos.index, 0);
    EXPECT_EQ(pos.qty_ahead, 0);
}

TEST(Append, Two){
    QueueOrder q;
    q.append(1, 3, 3);
    q.append(1, 4, 2);
    QueuePosition pos1 = q.query(1, 3);
    QueuePosition pos2 = q.query(1, 4);
    EXPECT_EQ(pos1.index, 0);
    EXPECT_EQ(pos1.qty_ahead, 0);
    EXPECT_EQ(pos2.index, 1);
    EXPECT_EQ(pos2.qty_ahead, 3);
}

TEST(Queue, Cancel){
    QueueOrder q;
    q.append(1, 3, 3);
    q.append(1, 4, 2);
    QueuePosition pos1 = q.query(1, 3);
    QueuePosition pos2 = q.query(1, 4);

    EXPECT_EQ(pos1.index, 0);
    EXPECT_EQ(pos1.qty_ahead, 0);
    EXPECT_EQ(pos2.index, 1);
    EXPECT_EQ(pos2.qty_ahead, 3);

    q.remove(1, 3);

    pos1 = q.query(1, 3);
    pos2 = q.query(1, 4);

    EXPECT_EQ(pos1.index, -1);
    EXPECT_EQ(pos1.qty_ahead, 0);
    EXPECT_EQ(pos2.index, 0);
    EXPECT_EQ(pos2.qty_ahead, 0);
}

TEST(Queue, ModifyQuantity){
    QueueOrder q;
    q.append(1, 3, 3);
    q.append(1, 4, 2);
    QueuePosition pos1 = q.query(1, 3);
    QueuePosition pos2 = q.query(1, 4);

    EXPECT_EQ(pos1.index, 0);
    EXPECT_EQ(pos1.qty_ahead, 0);
    EXPECT_EQ(pos2.index, 1);
    EXPECT_EQ(pos2.qty_ahead, 3);

    q.modify_quantity(1, 3, 5);

    pos1 = q.query(1, 3);
    pos2 = q.query(1, 4);

    EXPECT_EQ(pos1.index, 0);
    EXPECT_EQ(pos1.qty_ahead, 0);
    EXPECT_EQ(pos2.index, 1);
    EXPECT_EQ(pos2.qty_ahead, 5);
}

TEST(Queue, SumQuantity){
    QueueOrder q;
    q.append(1, 3, 3);
    q.append(1, 4, 2);
    q.append(1, 5, 6);
    QueuePosition pos = q.query(1, 5);

    EXPECT_EQ(pos.index, 2);
    EXPECT_EQ(pos.qty_ahead, 5);
}

