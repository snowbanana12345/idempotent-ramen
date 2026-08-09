#include "slot_heap_map.h"
#include <gmock/gmock.h>
#include <vector>


using namespace hftu;

using UTHeap = SlotMapHeaps<int, 2, 10> ;
using Cb = void(*)(int, int64_t);

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

TEST(Slotted, InsertSecondSlot){
    UTHeap h;
    h.insert(199, 15);
    EXPECT_EQ(h.first_event_time(), 15);
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
    static int fires = 0;

    Cb call_back = [](int id, int64_t t) { 
        fires++;
    };
    h.insert(199, 7);
    EXPECT_EQ(h.advance(6, call_back), 0);
    EXPECT_EQ(fires, 0);
    EXPECT_EQ(h.advance(7, call_back), 1);
    EXPECT_EQ(fires, 1);
    EXPECT_EQ(h.size(), 0);
    EXPECT_EQ(h.first_event_time(), INT64_MAX);
}

TEST(Slotted, InsertSameTime){
    UTHeap h;
    static int fires = 0;
    Cb call_back = [](int id, int64_t t) { 
        fires++;
    };

    h.insert(199, 15);
    h.insert(125, 15);
    EXPECT_EQ(h.advance(15, call_back), 2);
    EXPECT_EQ(fires, 2);
}

TEST(Slotted, RingRotation){
    UTHeap h;
     Cb call_back = [](int id, int64_t t) { };
    h.advance(10, call_back);
    EXPECT_EQ(h.start_time(), 10);
    EXPECT_EQ(h.end_time(), 30);
}

TEST(Slotted, FullRingRotation){
    UTHeap h;
    Cb call_back = [](int id, int64_t t) { };
    h.advance(35, call_back);
    EXPECT_EQ(h.start_time(), 30);
    EXPECT_EQ(h.end_time(), 50);
}

TEST(Slotted, FullRingRotationCorner1){
    UTHeap h;
    Cb call_back = [](int id, int64_t t) { };
    h.advance(39, call_back);
    EXPECT_EQ(h.start_time(), 30);
    EXPECT_EQ(h.end_time(), 50);
}

TEST(Slotted, FullRingRotationCorner2){
    UTHeap h;
    Cb call_back = [](int id, int64_t t) { };
    h.advance(40, call_back);
    EXPECT_EQ(h.start_time(), 40);
    EXPECT_EQ(h.end_time(), 60);
}

TEST(Slotted, SortedOrder){
    UTHeap h;
    static std::vector<int> times;
    Cb call_back = [](int id, int64_t t) { 
        times.push_back(t);
    };
    h.insert(1, 7);
    h.insert(2, 5);
    h.insert(3, 6);
    h.insert(4, 8);
    EXPECT_EQ(h.advance(8, call_back), 4);
    EXPECT_EQ(times.size(), 4);
    EXPECT_EQ(times[0], 5);
    EXPECT_EQ(times[1], 6);
    EXPECT_EQ(times[2], 7);
    EXPECT_EQ(times[3], 8);
}

TEST(Slotted, AdvancePastSlot){
    UTHeap h;
    static int fires = 0;
    Cb call_back = [](int id, int64_t t) { 
        fires++;
    };
    h.insert(199, 7);
    h.insert(125, 15);
    EXPECT_EQ(h.advance(15, call_back), 2);
    EXPECT_EQ(fires, 2);
}

TEST(Slotted, advance_earlier){
    UTHeap h;
    static int fires = 0;
    Cb call_back = [](int id, int64_t t) { 
        fires++;
    };
    EXPECT_EQ(h.advance(31, call_back), 0);
    EXPECT_EQ(h.start_time(), 30);
    EXPECT_EQ(h.end_time(), 50);
    h.insert(125, 35);
    EXPECT_EQ(h.advance(5, call_back), 0);
    EXPECT_EQ(h.first_event_time(), 35);
    EXPECT_EQ(h.size(), 1);
    EXPECT_EQ(h.start_time(), 30);
    EXPECT_EQ(h.end_time(), 50);
}