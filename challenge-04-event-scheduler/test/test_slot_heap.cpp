#include "slot_heap.h"
#include <gmock/gmock.h>
#include <vector>


using namespace hftu;

struct UTHeap : public SlotHeaps<int, UTHeap, 2, 10> {
    struct Record{
        int e;
        int64_t t;
    };

    std::vector<Record> fires;
    std::vector<Record> moves;

    void fire(int e, int64_t t) {
         fires.push_back({e, t});
    }

    void move(int e, int64_t t) {
         moves.push_back({e, t});
    }
};

TEST(Slotted, Empty){
    UTHeap h;
    EXPECT_EQ(h.start_time(), 0);
    EXPECT_EQ(h.end_time(), 20);
}

TEST(Slotted, Insert){
    UTHeap h;
    EXPECT_EQ(h.first_event_time(), INT64_MAX);
    EXPECT_EQ(h.size(), 0);
    h.insert(199, 3);
    EXPECT_EQ(h.first_event_time(), 3);
    EXPECT_EQ(h.size(), 1);
}

TEST(Slotted, InsertOutRange){
    UTHeap h;
    h.insert(199, 21);
    EXPECT_EQ(h.first_event_time(), INT64_MAX);
    EXPECT_EQ(h.size(), 0);
}

TEST(Slotted, Advance){
    UTHeap h;
    h.insert(199, 7);
    EXPECT_EQ(h.advance(6), 0);
    EXPECT_EQ(h.fires.size(), 0);
    EXPECT_EQ(h.advance(7), 1);
    EXPECT_EQ(h.fires.size(), 1);
    EXPECT_EQ(h.size(), 0);
    EXPECT_EQ(h.first_event_time(), INT64_MAX);
}


TEST(Slotted, InsertSameTime){
    UTHeap h;
    h.insert(199, 15);
    h.insert(125, 15);
    EXPECT_EQ(h.advance(15), 2);
}

TEST(Slotted, RingRotation){
    UTHeap h;
    h.advance(10);
    EXPECT_EQ(h.start_time(), 10);
    EXPECT_EQ(h.end_time(), 30);
}

TEST(Slotted, FullRingRotation){
    UTHeap h;
    h.advance(35);
    EXPECT_EQ(h.start_time(), 30);
    EXPECT_EQ(h.end_time(), 50);
}

TEST(Slotted, SortedOrder){
    UTHeap h;
    h.insert(1, 7);
    h.insert(2, 5);
    h.insert(3, 6);
    h.insert(4, 8);
    EXPECT_EQ(h.advance(8), 4);
    EXPECT_EQ(h.fires.size(), 4);
    EXPECT_EQ(h.fires[0].t, 5);
    EXPECT_EQ(h.fires[1].t, 6);
    EXPECT_EQ(h.fires[2].t, 7);
    EXPECT_EQ(h.fires[3].t, 8);
}

TEST(Slotted, AdvancePastSlot){
    UTHeap h;
    h.insert(199, 7);
    h.insert(125, 15);
    EXPECT_EQ(h.advance(15), 2);
}

TEST(Slotted, MoveCall){
    UTHeap h;
    h.insert(199, 7);
    EXPECT_EQ(h.advance(15, UTHeap::Op::MOVE), 1);
    EXPECT_EQ(h.fires.size(), 0);
    EXPECT_EQ(h.moves.size(), 1);
}