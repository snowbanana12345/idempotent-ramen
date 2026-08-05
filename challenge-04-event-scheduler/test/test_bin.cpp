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