#include "solution.h"
#include <gmock/gmock.h>
#include <vector>

using namespace hftu;

struct UTSchel : public hftu::EventScheduler<UTSchel> {
    struct Record{
        hftu::Event* e;
        int64_t t;
    };

    std::vector<Record> records;
    void fire(hftu::Event* e, int64_t t) {
         records.push_back({e, t});
    }
};

TEST(Schedule, One){
    UTSchel sch;
    Event event;
    sch.schedule(&event, 5);
    EXPECT_EQ(sch.size(), 1);
    EXPECT_EQ(sch.next_event_time(), 5);
    EXPECT_EQ(sch.advance(4), 0);
    EXPECT_EQ(sch.records.size(), 0);
    EXPECT_EQ(sch.advance(5), 1);
    EXPECT_EQ(sch.records.size(), 1);
    EXPECT_EQ(sch.records[0].e, &event);
}

TEST(Schedule, InOrder){
    UTSchel sch;
    Event event1;
    Event event2;
    Event event3;
    sch.schedule(&event1, 7);
    sch.schedule(&event2, 4);
    sch.schedule(&event3, 5);
    EXPECT_EQ(sch.advance(7), 3);
    EXPECT_EQ(sch.records.size(), 3);
    EXPECT_EQ(sch.records[0].e, &event2);
    EXPECT_EQ(sch.records[1].e, &event3);
    EXPECT_EQ(sch.records[2].e, &event1);
}

TEST(Schedule, NearThresholdFire){
    UTSchel sch;
    Event event1;
    Event event2;
    sch.schedule(&event1, 1);
    sch.schedule(&event2, 1007);
    EXPECT_EQ(sch.advance(1007), 2);
}

TEST(Schedule, NearThresholdMove){
    UTSchel sch;
    Event event1;
    Event event2;
    sch.schedule(&event1, 1);
    sch.schedule(&event2, 1007);
    EXPECT_EQ(sch.advance(1005), 1);
    EXPECT_EQ(sch.advance(1007), 1);
}

TEST(Schedule, FarThresholdFire){
    UTSchel sch;
    Event event1;
    Event event2;
    sch.schedule(&event1, 2000);
    sch.schedule(&event2, 7'000'000);
    EXPECT_EQ(sch.advance(7'000'000), 2);
}

TEST(Schedule, FarThresholdMove){
    UTSchel sch;
    Event event1;
    Event event2;
    Event event3;
    sch.schedule(&event1, 2000);
    sch.schedule(&event2, 1'001'000);
    sch.schedule(&event3, 1'001'001);
    EXPECT_EQ(sch.advance(1'000'999), 1);
    EXPECT_EQ(sch.advance(1'001'000), 1);
}