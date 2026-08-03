#include "solution.h"
#include <queue>
#include <vector>
#include <unordered_map>

namespace hftu{
    class Impl{
        public:
            Impl(){
                m_events.reserve(1'000'000);
            }

            void schedule(uint64_t event_id, int64_t time_us){
                m_pq.push({event_id, time_us});
                m_events[event_id] = time_us;

                 while (!m_pq.empty() && (m_events.find(m_pq.top().event_id) == m_events.end() || m_events[m_pq.top().event_id] != m_pq.top().time_us)
                ) {
                    m_pq.pop(); // Remove invalid events from the priority queue
                }
            }

            bool cancel(uint64_t event_id){
                uint64_t deleted = m_events.erase(event_id) > 0;
                while (!m_pq.empty() && 
                (m_events.find(m_pq.top().event_id) == m_events.end() || m_events[m_pq.top().event_id] != m_pq.top().time_us)
                ) {
                    m_pq.pop(); // Remove invalid events from the priority queue
                }
                return deleted;
            }

            uint32_t advance(int64_t new_time_us, EventCallback cb, void* user_data){
                uint32_t fired = 0;
                while(!m_pq.empty()){
                    auto event = m_pq.top();
                    if(event.time_us > new_time_us) break;
                    m_pq.pop();
                    if(m_events.find(event.event_id) != m_events.end() && m_events[event.event_id] == event.time_us){
                        cb(event.event_id, event.time_us, user_data);
                        ++fired;
                        m_events.erase(event.event_id);
                    }
                }
                return fired;
            }

            uint64_t size() const{
                return m_events.size();
            }

            int64_t next_event_time() const{
                if(m_pq.empty()) return INT64_MAX;
                return m_pq.top().time_us;
            }

        private:
            struct Event{
                uint64_t event_id;
                int64_t time_us;
            };

            struct EventComparator {
                bool operator()(const Event& a, const Event& b) const {
                    return a.time_us > b.time_us; 
                }
            };

            std::priority_queue<Event, std::vector<Event>, EventComparator> m_pq;
            std::unordered_map<uint64_t, int64_t> m_events;
    };
}

#include "pimpl.h"