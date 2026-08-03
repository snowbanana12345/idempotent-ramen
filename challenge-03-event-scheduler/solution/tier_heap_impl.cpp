#include "solution.h"
#include <queue>
#include <vector>
#include <unordered_set>

namespace hftu {
    constexpr int64_t NEAR_THRESHOLD = 1'000; // 1ms
    constexpr int64_t MID_THRESHOLD = 1'000'000; // 1s

    class Impl{
        public:
            Impl(){
                m_events.reserve(1'000'000);
            }

            void schedule(uint64_t event_id, int64_t time_us){
                if(time_us - m_curr_time < NEAR_THRESHOLD){
                    m_near_pq.push({event_id, time_us});
                } else if(time_us - m_curr_time < MID_THRESHOLD){
                    m_mid_pq.push({event_id, time_us});
                } else {
                    m_far_pq.push({event_id, time_us});
                }
                m_events.insert(event_id);
            }

            bool cancel(uint64_t event_id){
                uint64_t deleted = m_events.erase(event_id) > 0;
                while (!m_near_pq.empty() && m_events.find(m_near_pq.top().event_id) == m_events.end()) {
                    m_near_pq.pop();
                }
                while (!m_mid_pq.empty() && m_events.find(m_mid_pq.top().event_id) == m_events.end()) {
                    m_mid_pq.pop();
                }
                while (!m_far_pq.empty() && m_events.find(m_far_pq.top().event_id) == m_events.end()) {
                    m_far_pq.pop();
                }
                return deleted;
            }

            uint32_t advance(int64_t new_time_us, EventCallback cb, void* user_data){
                uint32_t fired = 0;
                m_curr_time = new_time_us;
                while(!m_near_pq.empty() && m_near_pq.top().time_us <= new_time_us){
                    auto event = m_near_pq.top();
                    m_near_pq.pop();
                    if(m_events.erase(event.event_id) > 0){
                        cb(event.event_id, event.time_us, user_data);
                        ++fired;
                    }
                }
                while(!m_mid_pq.empty() && m_mid_pq.top().time_us <= new_time_us){
                    auto event = m_mid_pq.top();
                    m_mid_pq.pop();
                    if(m_events.erase(event.event_id) > 0){
                        cb(event.event_id, event.time_us, user_data);
                        ++fired;
                    }
                }
                while(!m_far_pq.empty() && m_far_pq.top().time_us <= new_time_us){
                    auto event = m_far_pq.top();
                    m_far_pq.pop();
                    if(m_events.erase(event.event_id) > 0){
                        cb(event.event_id, event.time_us, user_data);
                        ++fired;
                    }
                }

                // ---- now we pour the mid events into near, and far into mid ----
                while(!m_mid_pq.empty() && m_mid_pq.top().time_us - m_curr_time < NEAR_THRESHOLD){
                    auto event = m_mid_pq.top();
                    m_mid_pq.pop();
                    m_near_pq.push(event);
                }
                while(!m_far_pq.empty() && m_far_pq.top().time_us - m_curr_time < MID_THRESHOLD){
                    auto event = m_far_pq.top();
                    m_far_pq.pop();
                    m_mid_pq.push(event);
                }
                
                return fired;
            }

            uint64_t size() const{
                return m_events.size();
            }

            int64_t next_event_time() const{
                if(!m_near_pq.empty()){
                    return m_near_pq.top().time_us;
                } else if(!m_mid_pq.empty()){
                    return m_mid_pq.top().time_us;
                } else if(!m_far_pq.empty()){
                    return m_far_pq.top().time_us;
                }
                return INT64_MAX;
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

            std::priority_queue<Event, std::vector<Event>, EventComparator> m_near_pq;
            std::priority_queue<Event, std::vector<Event>, EventComparator> m_mid_pq;
            std::priority_queue<Event, std::vector<Event>, EventComparator> m_far_pq;
            int64_t m_curr_time;
            std::unordered_set<uint64_t> m_events;
    };
}

#include "pimpl.h"