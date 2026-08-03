#include "solution.h"
#include <queue>
#include <set>
#include <unordered_map>
#include <unordered_set>

namespace hftu{
    constexpr uint32_t MICRO_SLOTS = 1000;
    constexpr uint32_t SLOT_SIZE = 2000;
    constexpr int64_t FAR_THRESHOLD = 1'000'000;

    class Impl{
        public:
            void schedule(uint64_t event_id, int64_t time_us){
                if(time_us - m_curr_time_us < MICRO_SLOTS){
                    insert_into_slots(event_id, time_us);
                } else if(time_us - m_curr_time_us < FAR_THRESHOLD){
                    m_mid_pq.push({event_id, time_us});
                } else {
                    m_far_pq.push({event_id, time_us});
                }

                m_events[event_id] = time_us;
            }

            bool cancel(uint64_t event_id){
                uint64_t deleted = m_events.erase(event_id) > 0;

                while (!m_overflow_buffer.empty() && !is_event_valid(m_overflow_buffer.top())) {
                    m_overflow_buffer.pop();
                }
                while (!m_mid_pq.empty() && !is_event_valid(m_mid_pq.top())) {
                    m_mid_pq.pop();
                }
                while (!m_far_pq.empty() && !is_event_valid(m_far_pq.top())) {
                    m_far_pq.pop();
                }

                return deleted;
            }

            uint32_t advance(int64_t new_time_us, EventCallback cb, void* user_data){
                uint32_t fired_count = 0;
                for (uint32_t i = 0; i < MICRO_SLOTS; ++i) {
                    uint32_t slot_index = (m_slot_ptr + i) % MICRO_SLOTS;
                    int64_t slot_time_us = m_curr_time_us + i;
                    if (slot_time_us > new_time_us) break;

                    for (uint32_t j = 0; j < m_inner_ptrs[slot_index]; ++j) {
                        uint64_t event_id = m_slots[slot_index][j];
                        if (m_events.find(event_id) != m_events.end() && m_events[event_id] == slot_time_us) {
                            cb(event_id, slot_time_us, user_data);
                            fired_count++;
                            m_events.erase(event_id);
                        }
                    }

                    while (!m_overflow_buffer.empty() && m_overflow_buffer.top().time_us <= new_time_us) {
                        Event event = m_overflow_buffer.top();
                        m_overflow_buffer.pop();
                        if (is_event_valid(event)) {
                            cb(event.event_id, event.time_us, user_data);
                            fired_count++;
                            m_events.erase(event.event_id);
                        }
                    }

                    m_inner_ptrs[slot_index] = 0; 
                }

                // ---- correctness for the case of new_time_us jumping milliseconds
                while (!m_mid_pq.empty() && m_mid_pq.top().time_us <= new_time_us) {
                    Event e = m_mid_pq.top();
                    m_mid_pq.pop();
                    if (m_events.find(e.event_id) != m_events.end() && m_events[e.event_id] == e.time_us) {
                        cb(e.event_id, e.time_us, user_data);
                        fired_count++;
                        m_events.erase(e.event_id);
                    }
                }

                while (!m_far_pq.empty() && m_far_pq.top().time_us <= new_time_us) {
                    Event e = m_far_pq.top();
                    m_far_pq.pop();
                    if (m_events.find(e.event_id) != m_events.end() && m_events[e.event_id] == e.time_us) {
                        cb(e.event_id, e.time_us, user_data);
                        fired_count++;
                        m_events.erase(e.event_id);
                    }
                }

                // ---- now we pour the mid events into near, and far into mid ----
                while(!m_mid_pq.empty() && m_mid_pq.top().time_us - m_curr_time_us < MICRO_SLOTS){
                    auto event = m_mid_pq.top();
                    m_mid_pq.pop();
                    if (is_event_valid(event)) insert_into_slots(event.event_id, event.time_us);
                }

                while(!m_far_pq.empty() && m_far_pq.top().time_us - m_curr_time_us < FAR_THRESHOLD){
                    auto event = m_far_pq.top();
                    m_far_pq.pop();
                    if (is_event_valid(event)) m_mid_pq.push(event);
                }

                return fired_count;
            }

            uint64_t size() const{
                return m_events.size();
            }
            
            int64_t next_event_time() const{
                if (!m_overflow_buffer.empty()) {
                    return m_overflow_buffer.top().time_us;
                }
                if (!m_mid_pq.empty()) {
                    return m_mid_pq.top().time_us;
                }
                if (!m_far_pq.empty()) {
                    return m_far_pq.top().time_us;
                }
                for (uint32_t i = 0; i < MICRO_SLOTS; ++i) {
                    uint32_t slot_index = (m_slot_ptr + i) % MICRO_SLOTS;
                    if (m_inner_ptrs[slot_index] > 0) {
                        return m_curr_time_us + i;
                    }
                }
                return INT64_MAX; // No events scheduled
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

            uint64_t m_slots[MICRO_SLOTS][SLOT_SIZE];
            uint32_t m_slot_ptr;
            uint32_t m_inner_ptrs[MICRO_SLOTS];

            int64_t m_curr_time_us;

            std::unordered_map<uint64_t, int64_t> m_events;
            std::priority_queue<Event, std::vector<Event>, EventComparator> m_overflow_buffer;
            std::priority_queue<Event, std::vector<Event>, EventComparator> m_mid_pq;
            std::priority_queue<Event, std::vector<Event>, EventComparator> m_far_pq;

            void insert_into_slots(uint64_t event_id, int64_t time_us){
                uint32_t offset = static_cast<uint32_t>(time_us - m_curr_time_us);
                uint32_t slot_index = (m_slot_ptr + offset) % MICRO_SLOTS;
                uint32_t inner_index = m_inner_ptrs[slot_index];
                if (inner_index >= SLOT_SIZE) {
                    // Overflow, push to overflow buffer
                    m_overflow_buffer.push({event_id, time_us});
                } else {
                    m_slots[slot_index][inner_index] = event_id;
                    m_inner_ptrs[slot_index]++;
                }
            }

            inline bool is_event_valid(Event event) const {
                auto it = m_events.find(event.event_id);
                return it != m_events.end() && it->second == event.time_us;
            }
    };
}

#include "pimpl.h"