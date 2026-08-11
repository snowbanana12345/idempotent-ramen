#include "base.h"
#include "slot_heap_map.h"

namespace hftu {
constexpr uint32_t NEAR_SLOTS = 16;
constexpr int64_t NEAR_INTERVAL = 64; // ~ 1us

constexpr int64_t MICRO_SECOND = 1024;
constexpr int64_t MILLI_SECOND = 1024 * 1024;
constexpr int64_t SECOND = 1024 * 1024 * 1024;

constexpr uint32_t MID_SLOTS = 16;
constexpr int64_t MID_INTERVAL = 64 * MILLI_SECOND; // 100ms

constexpr uint32_t FAR_SLOTS = 64;
constexpr int64_t FAR_INTERVAL = SECOND; // 60 second

    class EventScheduler {
        public:
            EventScheduler(){

            }

            void schedule(uint64_t event_id, int64_t time_us){
                m_near_heap.remove(event_id);
                m_mid_heap.remove(event_id);
                m_far_heap.remove(event_id);
                
                if (time_us < m_near_heap.end_time()){
                    m_near_heap.insert(event_id, time_us);
                }
                else if (time_us < m_mid_heap.end_time()){
                    m_mid_heap.insert(event_id, time_us);   
                }
                else {
                    m_far_heap.insert(event_id, time_us);   
                }
            }

            bool cancel(uint64_t event_id){
                bool removed = false;
                
                removed |= m_near_heap.remove(event_id);
                removed |= m_far_heap.remove(event_id);
                removed |= m_mid_heap.remove(event_id);

                return removed;
            }

            uint32_t advance(int64_t new_time_us, EventCallback cb, void* user_data){
                std::function<void(uint64_t, int64_t)> fire_call_back = [&cb, user_data](uint64_t event_id, int64_t time_us){
                    cb(event_id, time_us, user_data);
                };

                uint32_t fired = 0;

                fired += m_near_heap.advance(new_time_us, fire_call_back);
                fired += m_mid_heap.advance(new_time_us, fire_call_back);
                fired += m_far_heap.advance(new_time_us, fire_call_back);

                if (m_near_heap.end_time() >= m_mid_heap.start_time()){
                    m_mid_heap.advance(m_near_heap.end_time(), m_mid_move);
                }

                if(m_mid_heap.end_time() >= m_far_heap.start_time()){
                    m_far_heap.advance(m_mid_heap.end_time(), m_far_move);
                }

                return fired;
            }

            uint64_t size() const{
                return m_near_heap.size() + m_mid_heap.size() + m_far_heap.size();
            }

            int64_t next_event_time() const{
                if (m_near_heap.size() > 0) return m_near_heap.first_event_time();
                if (m_mid_heap.size() > 0) return m_mid_heap.first_event_time();
                if (m_far_heap.size() > 0) return m_far_heap.first_event_time();
                return INT64_MAX;
            }

        private:
            SlotMapHeaps<uint64_t, NEAR_SLOTS, NEAR_INTERVAL> m_near_heap;
            SlotMapHeaps<uint64_t, MID_SLOTS, MID_INTERVAL> m_mid_heap;
            SlotMapHeaps<uint64_t, FAR_SLOTS, FAR_INTERVAL> m_far_heap;  

            std::function<void(uint64_t, int64_t)> m_mid_move = [this](uint64_t event_id, int64_t time_us){
                m_near_heap.insert(event_id, time_us);
            };

            std::function<void(uint64_t, int64_t)> m_far_move = [this](uint64_t event_id, int64_t time_us){
                m_mid_heap.insert(event_id, time_us);
            };
    };
}