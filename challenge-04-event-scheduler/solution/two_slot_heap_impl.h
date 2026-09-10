#include "slot_heap.h"
#include <functional>


namespace hftu{
struct Event {};

constexpr uint32_t HOT_SLOTS = 16;
constexpr int64_t HOT_INTERVAL = 64; // ~ 1us

constexpr int64_t MICRO_SECOND = 1024;
constexpr int64_t MILLI_SECOND = 1024 * 1024;
constexpr int64_t SECOND = 1024 * 1024 * 1024;

constexpr uint32_t COLD_SLOTS = 64;
constexpr int64_t COLD_INTERVAL = SECOND; // 60 second

using CallBack = std::function<void(Event*, int64_t)>;


template <typename Derived>
class EventScheduler {

public:
    EventScheduler(){}

    Derived* me() { return static_cast<Derived*>(this); }

    void schedule(Event* event, int64_t time_ns) {
        if (time_ns < m_hot_heap.end_time()){
            m_hot_heap.insert(event, time_ns);
        }
        else {
            m_cold_heap.insert(event, time_ns);
        }
    }

    uint32_t advance(int64_t new_time_ns) {
        uint32_t fired = 0;
        CallBack event_cb = [this](Event* event, int64_t t) { 
            me()->fire(event, t);
        };

        fired += m_hot_heap.advance(new_time_ns, event_cb);
        fired += m_cold_heap.advance(new_time_ns, event_cb);

        if (m_cold_heap.start_time() < m_hot_heap.end_time()){
            CallBack cold_move_cb = [this](Event* event, int64_t t) { 
                m_hot_heap.insert(event, t);
            };
            m_cold_heap.advance(m_hot_heap.end_time(), cold_move_cb);
        }
        
        m_curr_time = new_time_ns;
        return fired;
    }

    uint64_t size() const {  
        return m_hot_heap.size() + m_cold_heap.size();
    }

    int64_t next_event_time() const {
        if (m_hot_heap.size() > 0) return m_hot_heap.first_event_time();
        if (m_cold_heap.size() > 0) return m_cold_heap.first_event_time();
        return INT64_MAX;
    }

private:
    SlotHeaps<Event*, HOT_SLOTS, HOT_INTERVAL> m_hot_heap;
    SlotHeaps<Event*, COLD_SLOTS, COLD_INTERVAL> m_cold_heap;

    int64_t m_curr_time = 0;
};

}