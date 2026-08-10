#include "base.h"


namespace hftu{

constexpr uint32_t MICRO_SLOTS = 1024;


class EventScheduler {
        public:
            EventScheduler(){

            }

            ~EventScheduler(){

            }

            void schedule(uint64_t event_id, int64_t time_us){
                auto it = m_events.find(event_id);

                if (it != m_events.end() && it->second < m_curr_time + MICRO_SLOTS){ // reschedule
                    delete_slot(event_id, it->second);
                }
                while (!m_cold_store.empty() && m_cold_store.top().t != m_events[m_cold_store.top().id]){
                    m_cold_store.pop();
                }

                m_events[event_id] = time_us;

                if (time_us < m_curr_time + MICRO_SLOTS){
                   insert_into_slots(event_id, time_us);
                }
                else {
                    m_cold_store.push({event_id, time_us});
                }
            }

            bool cancel(uint64_t event_id){
                auto it = m_events.find(event_id);
                bool found = it != m_events.end();
                if (found && it->second < m_curr_time + MICRO_SLOTS){
                    delete_slot(event_id, it->second);
                }
                if (found) m_events.erase(it);
                while (!m_cold_store.empty() && m_cold_store.top().t != m_events[m_cold_store.top().id]){
                    m_cold_store.pop();
                }
                return found;
            }

            uint32_t advance(int64_t new_time_us, EventCallback cb, void* user_data){
                uint32_t fired = 0;
                // ---- fire off hot events ----
                for (int i = 0; i < MICRO_SLOTS && m_curr_time + i <= new_time_us; i++){
                    uint32_t ind = (m_slot_ptr + i) % MICRO_SLOTS;
                    auto& mp = m_slots[ind];
                    
                    for (uint64_t event_id : mp){
                        cb(event_id, m_curr_time + i, user_data);
                        m_events.erase(event_id);
                    }
                    fired += mp.size();
                    mp.clear();
                }

                // ---- fire off cold events ----
                while (!m_cold_store.empty() && m_cold_store.top().t <= new_time_us){
                    Event e = m_cold_store.top(); m_cold_store.pop();
                    if (m_events[e.id] == e.t){
                        cb(e.id, e.t, user_data);
                        m_events.erase(e.id);
                        fired++;
                    }
                }

                m_slot_ptr = (m_slot_ptr + static_cast<uint32_t>(new_time_us - m_curr_time)) % MICRO_SLOTS;
                m_curr_time = new_time_us;

                // ---- move cold events into hot store -----
                while (!m_cold_store.empty() && m_cold_store.top().t <= m_curr_time + MICRO_SLOTS){
                    Event e = m_cold_store.top(); m_cold_store.pop();
                    if (m_events[e.id] == e.t){
                        insert_into_slots(e.id, e.t);
                    }
                }

                return fired;
            }

            uint64_t size() const{
                return m_events.size();
            }

            int64_t next_event_time() const{
                for (uint32_t i = 0; i < MICRO_SLOTS; i++){
                    uint32_t ind = (m_slot_ptr + i) % MICRO_SLOTS;
                    auto& mp = m_slots[ind];
                    if (!mp.empty()) return m_curr_time + i;
                }
                
                if (!m_cold_store.empty()) return m_cold_store.top().t;
                return INT64_MAX;
            }

        private:
            struct Event{
                uint64_t id;
                int64_t t;
            };

            struct EventComparator {
                bool operator()(const Event& a, const Event& b) const {
                    return a.t > b.t; 
                }
            };

            std::priority_queue<Event, std::vector<Event>, EventComparator> m_cold_store;
            std::unordered_map<uint64_t, int64_t> m_events; 

            uint32_t m_slot_ptr = 0;
            std::unordered_set<uint64_t> m_slots[MICRO_SLOTS];
            
            int64_t m_curr_time = 0;

            void insert_into_slots(uint64_t event_id, int64_t time_us){
                uint32_t offset = static_cast<uint32_t>(time_us - m_curr_time);
                uint32_t ind = (m_slot_ptr + offset) % MICRO_SLOTS;
                m_slots[ind].emplace(event_id);
            }

            void delete_slot(uint64_t event_id, int64_t time_us){
                uint32_t offset = static_cast<uint32_t>(time_us - m_curr_time);
                uint32_t ind = (m_slot_ptr + offset) % MICRO_SLOTS;
                m_slots[ind].erase(event_id); 
            }
    };
}
