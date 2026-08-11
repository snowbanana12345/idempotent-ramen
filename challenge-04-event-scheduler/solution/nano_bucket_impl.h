#include "nano_bucket.h"
#include "queue"

namespace hftu{
    struct Event {};

    constexpr uint32_t BUCKETS = 1024;

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
        EventScheduler() : m_buckets(1000){

        }
        Derived* me() { return static_cast<Derived*>(this); }

        void schedule(Event* event, int64_t time_ns) {
            if (time_ns < m_buckets.end_time()){
                m_buckets.insert(event, time_ns);
            }
            else{
                m_cold_store.push({event, time_ns});
            }
        }

        uint32_t advance(int64_t new_time_ns) {
            std::function<void(Event*, int64_t)> event_cb = [this](Event* event, int64_t t) { 
                me()->fire(event, t);
            };

            uint32_t fired = 0;
            
            fired += m_buckets.advance(new_time_ns, event_cb);

            while (!m_cold_store.empty() && m_cold_store.top().t  <= new_time_ns){
                Record record = m_cold_store.top(); m_cold_store.pop();
                me()->fire(record.e, record.t);
                fired++;
            }

            while (!m_cold_store.empty() && m_cold_store.top().t <= m_buckets.end_time()){
                Record record = m_cold_store.top(); m_cold_store.pop();
                m_buckets.insert(record.e, record.t);
            }

            return fired;
        }

        uint64_t size() const { 
            return m_buckets.size() + m_cold_store.size();
        }

        int64_t next_event_time() const {
            if (m_buckets.size() > 0) return m_buckets.first_event_time();
            if (!m_cold_store.empty()) return m_cold_store.top().t;
            return INT64_MAX;
        }

    private:
        NanoBuckets<Event*, BUCKETS> m_buckets;
        std::priority_queue<Record, std::vector<Record>, RecordComparator> m_cold_store;
    };
}