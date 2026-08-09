#include <queue>
#include <functional>

namespace hftu{
    

    template <typename T, uint32_t SLOTS, int64_t INTERVAL>
    class SlotHeaps{
        using CallBack = std::function<void(T, int64_t)>;

        public:
            SlotHeaps(){
                m_curr_time = 0;
                m_slot_ptr = 0;
                for (uint32_t i = 0; i < SLOTS; i++){
                    m_slots[i].start_time = m_curr_time + i * INTERVAL;
                    m_slots[i].end_time = m_curr_time + (i + 1) * INTERVAL;
                }
            }

            int64_t start_time() const{
                return m_slots[m_slot_ptr].start_time;
            }
            int64_t end_time() const{
                return start_time() + SLOTS * INTERVAL;
            }

            void insert(T value, int64_t time_ns){
                uint32_t offset = (time_ns - start_time()) / INTERVAL;
                uint32_t ind = (m_slot_ptr + offset) % SLOTS;
                m_slots[ind].pq.push({value, time_ns});
            }

            int64_t first_event_time() const{
                if (m_slots[m_slot_ptr].pq.empty()) return INT64_MAX;
                return m_slots[m_slot_ptr].pq.top().t;
            }

            uint32_t size() const{
                uint32_t size_ = 0;

                for (uint32_t i = 0; i < SLOTS; i++){
                    size_ += m_slots[i].pq.size();
                }

                return size_;
            }

            uint32_t advance(int64_t time_ns, CallBack call_back){
                if (time_ns >= this->end_time()){
                    return fire_all(time_ns, call_back);
                }
                return fire_some(time_ns, call_back);
            }

        private:
            struct Timed{
                T value;
                int64_t t;
            };    

            struct Compare {
                bool operator()(const Timed& a, const Timed& b) const {
                    return a.t > b.t; 
                }
            };

            struct Slot{
                int64_t start_time;
                int64_t end_time;
                std::priority_queue<Timed, std::vector<Timed>, Compare> pq;
            };

            int64_t m_curr_time;
            uint32_t m_slot_ptr;
            Slot m_slots[SLOTS];

            uint32_t fire_all(int64_t time_ns, CallBack call_back){
                uint32_t fired = this->size();
                for (int i = 0; i < SLOTS; i++) {
                    auto &pq = m_slots[m_slot_ptr].pq;
                    while (!pq.empty()){
                        Timed t = pq.top(); pq.pop();
                        call_back(t.value, t.t);
                    }
                    
                    m_slots[m_slot_ptr].start_time += SLOTS * INTERVAL;
                    m_slots[m_slot_ptr].end_time += SLOTS * INTERVAL;
                    m_slot_ptr = (m_slot_ptr + 1) % SLOTS;
                }

                m_slot_ptr = 0;
                int64_t new_start_time = (time_ns / INTERVAL) * INTERVAL;
                
                for (int i = 0; i < SLOTS; i++){
                    m_slots[i].start_time = new_start_time + i * INTERVAL;
                    m_slots[i].end_time = new_start_time + (i + 1) * INTERVAL;
                }

                return fired;
            }

            uint32_t fire_some(int64_t time_ns, CallBack call_back){
                uint32_t fired = 0;
                for (int i = 0; i < SLOTS; i++) {
                    auto &pq = m_slots[m_slot_ptr].pq;
                    while (!pq.empty() && pq.top().t <= time_ns){
                        Timed t = pq.top(); pq.pop();
                        call_back(t.value, t.t);
                        ++fired;
                    }
                    
                    if (time_ns < m_slots[m_slot_ptr].end_time) break;
                
                    m_slots[m_slot_ptr].start_time += SLOTS * INTERVAL;
                    m_slots[m_slot_ptr].end_time += SLOTS * INTERVAL;
                    m_slot_ptr = (m_slot_ptr + 1) % SLOTS;
                }

                return fired;
            }
    };
}