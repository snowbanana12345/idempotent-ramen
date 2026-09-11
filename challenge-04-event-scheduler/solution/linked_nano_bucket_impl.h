#include "linked_nano_bucket.h"
#include "slot_heap.h"
#include "queue"

namespace hftu{
    struct Event {};

    constexpr uint32_t BUCKETS = 1024;
    constexpr uint32_t COLD_SLOTS = 256; 
    constexpr uint32_t SEGMENT_LENGTH = 64;

    constexpr int64_t MILLI_SECOND = 1024 * 1024;
    constexpr int64_t COLD_INTERVAL = 256 * MILLI_SECOND;

    struct Record{
        Event* e;
        int64_t t;
    };

    struct RecordComparator {
        bool operator()(const Record& a, const Record& b) const {
            return a.t > b.t; 
        }
    };

    template <typename Derived>
    class EventScheduler {
    public:
        EventScheduler() : m_buckets(2000'000){

        }
        Derived* me() { return static_cast<Derived*>(this); }

        void schedule(Event* event, int64_t time_ns) {
            if (time_ns < m_buckets.end_time()){
                m_buckets.insert(event, time_ns);
            }
            else if (time_ns < m_cold.end_time()){
                m_cold.insert(event, time_ns);
            }
        }

        uint32_t advance(int64_t new_time_ns) {
            std::function<void(Event*, int64_t)> event_cb = [this](Event* event, int64_t t) { 
                me()->fire(event, t);
            };

            uint32_t fired = 0;
            
            fired += m_buckets.advance(new_time_ns, event_cb);
            fired += m_cold.advance(new_time_ns, event_cb);

            std::function<void(Event*, int64_t)> event_move = [this](Event* event, int64_t t) { 
                m_buckets.insert(event, t);
            };

            m_cold.advance(m_buckets.end_time(), event_move);

            return fired;
        }

        uint64_t size() const { 
            return m_buckets.size() + m_cold.size();
        }

        int64_t next_event_time() const {
            if (m_buckets.size() > 0) return m_buckets.first_event_time();
            if (m_cold.size() > 0) return m_cold.first_event_time();
            return INT64_MAX;
        }

    private:
        LinkedNanoBuckets<Event*, BUCKETS, SEGMENT_LENGTH> m_buckets;
        SlotHeaps<Event*, COLD_SLOTS, COLD_INTERVAL> m_cold;
    };
}