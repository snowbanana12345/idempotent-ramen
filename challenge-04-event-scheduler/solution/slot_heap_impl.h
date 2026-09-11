#include "slot_heap.h"
#include <functional>


namespace hftu{
struct Event {};

constexpr uint32_t NEAR_SLOTS = 16;
constexpr int64_t NEAR_INTERVAL = 64; // ~ 1us

constexpr int64_t MICRO_SECOND = 1024;
constexpr int64_t MILLI_SECOND = 1024 * 1024;
constexpr int64_t SECOND = 1024 * 1024 * 1024;

constexpr uint32_t MID_SLOTS = 16;
constexpr int64_t MID_INTERVAL = 64 * MILLI_SECOND; // 100ms

constexpr uint32_t FAR_SLOTS = 64;
constexpr int64_t FAR_INTERVAL = SECOND; // 60 second

using CallBack = std::function<void(Event*, int64_t)>;


template <typename Derived>
class EventScheduler {

public:
    EventScheduler(){}

    Derived* me() { return static_cast<Derived*>(this); }

    void schedule(Event* event, int64_t time_ns) {
        if (time_ns < m_near_heap.end_time()){
            m_near_heap.insert(event, time_ns);
        }
        else if (time_ns < m_mid_heap.end_time()){
            m_mid_heap.insert(event, time_ns);
        }
        else {
            m_far_heap.insert(event, time_ns);
        }
    }

    uint32_t advance(int64_t new_time_ns) {
        uint32_t fired = 0;
        CallBack event_cb = [this](Event* event, int64_t t) { 
            me()->fire(event, t);
        };

        fired += m_near_heap.advance(new_time_ns, event_cb);
        fired += m_mid_heap.advance(new_time_ns, event_cb);
        fired += m_far_heap.advance(new_time_ns, event_cb);

        if (m_mid_heap.start_time() < m_near_heap.end_time()){
            // move mid_heap interval to start_heap_interval
            CallBack mid_move_cb = [this](Event* event, int64_t t) { 
                m_near_heap.insert(event, t);
            };
            m_mid_heap.advance(m_near_heap.end_time(), mid_move_cb);
        }

        if (m_far_heap.start_time() < m_mid_heap.end_time()){
            // move far_heap_interval into mid_heap_interval
            CallBack far_move_cb = [this](Event* event, int64_t t) { 
                m_mid_heap.insert(event, t);
            };
            m_far_heap.advance(m_mid_heap.end_time(), far_move_cb);
        }

        m_curr_time = new_time_ns;
        return fired;
    }

    uint64_t size() const {  
        return m_near_heap.size() + m_mid_heap.size() + m_far_heap.size();
    }

    int64_t next_event_time() const {
        if (m_near_heap.size() > 0) return m_near_heap.first_event_time();
        if (m_mid_heap.size() > 0) return m_mid_heap.first_event_time();
        if (m_far_heap.size() > 0) return m_far_heap.first_event_time();
        return INT64_MAX;
    }

private:
    SlotHeaps<Event*, NEAR_SLOTS, NEAR_INTERVAL> m_near_heap;
    SlotHeaps<Event*, MID_SLOTS, MID_INTERVAL> m_mid_heap;
    SlotHeaps<Event*, FAR_SLOTS, FAR_INTERVAL> m_far_heap;  

    int64_t m_curr_time = 0;
};

}