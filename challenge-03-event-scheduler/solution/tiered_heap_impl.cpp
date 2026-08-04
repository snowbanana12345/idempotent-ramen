#include "solution.h"
#include <queue>
#include <vector>
#include <unordered_set>

namespace hftu {
    constexpr uint32_t NUMBER_SLOTS = 10;
    constexpr int64_t SLOT_DURATION_US = 100; // 100us
    constexpr int64_t NEAR_THRESHOLD = NUMBER_SLOTS * SLOT_DURATION_US; // 1ms
    constexpr int64_t MID_THRESHOLD = 1'000'000; // 1s

    struct Event{
        uint64_t event_id;
        int64_t time_us;
    };

    struct EventComparator {
        bool operator()(const Event& a, const Event& b) const {
            return a.time_us > b.time_us; 
        }
    };

    struct PqSlot{
        int64_t start_time; // inclusive
        int64_t end_time; // exclusive
        std::priority_queue<Event, std::vector<Event>, EventComparator> pq;
    };

    class Impl{
        public:
            Impl(){
                m_curr_slot = 0;
                m_curr_time = 0;
                m_events.reserve(1'000'000);
                for (int i = 0; i < NUMBER_SLOTS; ++i) {
                    m_pq_slots[i].start_time = i * SLOT_DURATION_US;
                    m_pq_slots[i].end_time = (i + 1) * SLOT_DURATION_US;
                }
            }

            void schedule(uint64_t event_id, int64_t time_us){
                if(time_us - m_curr_time < NEAR_THRESHOLD){
                    insert_into_slots({event_id, time_us});
                } else if(time_us - m_curr_time < MID_THRESHOLD){
                    m_mid_pq.push({event_id, time_us});
                } else {
                    m_far_pq.push({event_id, time_us});
                }
                m_events[event_id] = time_us;
                clear_invalid_tops();
            }

            bool cancel(uint64_t event_id){
                uint64_t deleted = m_events.erase(event_id) > 0;
                clear_invalid_tops();
                return deleted;
            }

            uint32_t advance(int64_t new_time_us, EventCallback cb, void* user_data){
                uint32_t fired = 0;
                m_curr_time = new_time_us;
                
                for (int i = 0; i < NUMBER_SLOTS; i++){
                    auto &pq = m_pq_slots[m_curr_slot].pq;
                    while (!pq.empty() && pq.top().time_us <= new_time_us){
                        auto event = pq.top();
                        pq.pop();
                        if(is_event_valid(event)){
                            cb(event.event_id, event.time_us, user_data);
                            ++fired;
                            m_events.erase(event.event_id);
                        }
                    }
                    
                    // --- advance the slot pointer if time past the first slot ---
                    if (new_time_us < m_pq_slots[m_curr_slot].end_time) break;
                    
                    // the priority_queue should already be empty
                    m_pq_slots[m_curr_slot].start_time += NEAR_THRESHOLD;
                    m_pq_slots[m_curr_slot].end_time += NEAR_THRESHOLD;
                    m_curr_slot = (m_curr_slot + 1) % NUMBER_SLOTS;
                }

                while(!m_mid_pq.empty() && m_mid_pq.top().time_us <= new_time_us){
                    auto event = m_mid_pq.top();
                    m_mid_pq.pop();
                    if(is_event_valid(event)){
                        cb(event.event_id, event.time_us, user_data);
                        ++fired;
                        m_events.erase(event.event_id);
                    }
                }

                while(!m_far_pq.empty() && m_far_pq.top().time_us <= new_time_us){
                    auto event = m_far_pq.top();
                    m_far_pq.pop();
                    if(is_event_valid(event)){
                        cb(event.event_id, event.time_us, user_data);
                        ++fired;
                        m_events.erase(event.event_id);
                    }
                }

                // ---- now we pour the mid events into near, and far into mid ----
                while(!m_mid_pq.empty() && m_mid_pq.top().time_us - m_curr_time < NEAR_THRESHOLD){
                    auto event = m_mid_pq.top();
                    m_mid_pq.pop();
                    insert_into_slots(event);
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
                for (int i = 0; i < NUMBER_SLOTS; i++){
                    auto &pq = m_pq_slots[(m_curr_slot + i) % NUMBER_SLOTS].pq;
                    if (!pq.empty()){
                        return pq.top().time_us;
                    }
                }

                if(!m_mid_pq.empty()){
                    return m_mid_pq.top().time_us;
                }
                if (!m_far_pq.empty()){
                    return m_far_pq.top().time_us;
                }
                return INT64_MAX;
            }

        private:
            PqSlot m_pq_slots[NUMBER_SLOTS];
            uint32_t m_curr_slot;
            int64_t m_curr_time;

            std::priority_queue<Event, std::vector<Event>, EventComparator> m_mid_pq;
            std::priority_queue<Event, std::vector<Event>, EventComparator> m_far_pq;
            std::unordered_map<uint64_t, int64_t> m_events;
            
            void insert_into_slots(Event event){
                for (int i = 0; i < NUMBER_SLOTS; i++){
                    PqSlot& slot = m_pq_slots[(m_curr_slot + i) % NUMBER_SLOTS];
                    if (slot.start_time <= event.time_us && event.time_us < slot.end_time){
                        slot.pq.push(event); break;
                    }
                }
            }

            inline bool is_event_valid(Event event) const {
                auto it = m_events.find(event.event_id);
                return it != m_events.end() && it->second == event.time_us;
            }

            void clear_invalid_tops(){
                for (int i = 0; i < NUMBER_SLOTS; i++){
                    auto &pq = m_pq_slots[i].pq;
                    while (!pq.empty() && !is_event_valid(pq.top())){
                        pq.pop();
                    }
                }

                while (!m_mid_pq.empty() && !is_event_valid(m_mid_pq.top())) {
                    m_mid_pq.pop();
                }
                while (!m_far_pq.empty() && !is_event_valid(m_far_pq.top())) {
                    m_far_pq.pop();
                }
            }
    };
}

#include "pimpl.h"