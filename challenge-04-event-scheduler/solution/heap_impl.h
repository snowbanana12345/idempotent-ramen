#include <queue>
#include <iostream>

namespace hftu{
struct Event {};

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
    EventScheduler(){}
    Derived* me() { return static_cast<Derived*>(this); }

    void schedule(Event* event, int64_t time_ns) {
        m_pq.push({event, time_ns});
    }

    uint32_t advance(int64_t new_time_ns) {
        uint32_t fired = 0;
        while (!m_pq.empty() && m_pq.top().t  <= new_time_ns){
            Record record = m_pq.top(); m_pq.pop();
            me()->fire(record.e, record.t);
            fired++;
        }
        return fired;
    }

    uint64_t size() const { return m_pq.size(); }

    int64_t next_event_time() const {
        return m_pq.empty() ? INT64_MAX : m_pq.top().t; 
    }

private:
    std::priority_queue<Record, std::vector<Record>, RecordComparator> m_pq;
};
}