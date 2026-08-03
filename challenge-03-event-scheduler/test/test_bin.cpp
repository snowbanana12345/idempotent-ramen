#include "solution.h"
#include <gmock/gmock.h>
#include <array>


/*
class EventScheduler {
public:
    EventScheduler() = default;
    ~EventScheduler() = default;

    void schedule(uint64_t event_id, int64_t time_us);
    bool cancel(uint64_t event_id);
    uint32_t advance(int64_t new_time_us, EventCallback cb, void* user_data);
    uint64_t size() const;
    int64_t next_event_time() const;
*/

using namespace hftu;

TEST(Schedule, One){
    EventScheduler scheduler;
    scheduler.schedule(7, 5);
    EXPECT_EQ(scheduler.size(), 1);
    EXPECT_EQ(scheduler.next_event_time(), 5);
}

TEST(Query, Empty){
    EventScheduler scheduler;
    EXPECT_EQ(scheduler.size(), 0);
    EXPECT_EQ(scheduler.next_event_time(), INT64_MAX);
    EventCallback cb = [](uint64_t event_id, int64_t scheduled_time, void* user_data) { 
        EXPECT_TRUE(false);
    };
    EXPECT_EQ(scheduler.advance(8, cb, nullptr), 0);
}

TEST(Cancel, Basic){
    EventScheduler scheduler;
    scheduler.schedule(7, 5);
    EXPECT_EQ(scheduler.size(), 1);
    EXPECT_EQ(scheduler.next_event_time(), 5);
    EXPECT_TRUE(scheduler.cancel(7));
    EXPECT_EQ(scheduler.size(), 0);
    EXPECT_EQ(scheduler.next_event_time(), INT64_MAX);
}

TEST(Cancel, Empty){
    EventScheduler scheduler;
    EXPECT_FALSE(scheduler.cancel(100));
}

TEST(Advance, One){
    EventScheduler scheduler;
    scheduler.schedule(9, 7);
    EventCallback cb = [](uint64_t event_id, int64_t scheduled_time, void* user_data) { 
        EXPECT_EQ(event_id, 9);
        EXPECT_EQ(scheduled_time, 7);
    };
    EXPECT_EQ(scheduler.advance(6, cb, nullptr), 0);
    EXPECT_EQ(scheduler.advance(7, cb, nullptr), 1);
}

namespace adv_test::in_order{
    static std::array<uint64_t, 4> event_ids = {7, 4, 11, 10};
    static std::array<uint64_t, 4> times = {5, 6, 9, 11};
    static int g_ptr = 0;

    static void callbackImpl(uint64_t event_id, int64_t scheduled_time, void* user_data) {
        EXPECT_EQ(event_ids[g_ptr], event_id);
        EXPECT_EQ(times[g_ptr], scheduled_time);
        g_ptr++;
    }
}

TEST(Advance, InOrder){
    EventScheduler scheduler;
    using namespace adv_test::in_order; 
    
    scheduler.schedule(event_ids[0], times[0]);
    scheduler.schedule(event_ids[1], times[1]);
    scheduler.schedule(event_ids[2], times[2]);
    scheduler.schedule(event_ids[3], times[3]);

    EventCallback cb = callbackImpl;

    EXPECT_EQ(scheduler.advance(15, cb, nullptr), 4);
}

namespace adv_test::far{
    static std::array<uint64_t, 4> event_ids = {7, 8, 11};
    static std::array<uint64_t, 4> times = {5, 1001, 1'000'300};
    static int g_ptr = 0;

    static void callbackImpl(uint64_t event_id, int64_t scheduled_time, void* user_data) {
        EXPECT_EQ(event_ids[g_ptr], event_id);
        EXPECT_EQ(times[g_ptr], scheduled_time);
        g_ptr++;
    }
}

TEST(Advance, Far){
    EventScheduler scheduler;
    using namespace adv_test::far; 
    
    scheduler.schedule(event_ids[0], times[0]);
    scheduler.schedule(event_ids[1], times[1]);
    scheduler.schedule(event_ids[2], times[2]);

    EventCallback cb = callbackImpl;

    EXPECT_EQ(scheduler.advance(2'000'000, cb, nullptr), 3);
}