#include <queue>
#include <optional>

namespace hftu{
    template <typename T, typename Derived, uint32_t SLOTS, uint64_t INTERVAL>
    class SlottedHeaps{
        public:
            SlottedHeaps(){

            }

            Derived* me() { return static_cast<Derived*>(this); }

            uint64_t start_time(){
                return slots[m_slot_ptr].start_time;
            }
            uint64_t end_time(){
                return start_time() + SLOTS * INTERVAL;
            }

            void insert(T value){
                for (int i = 0; i < SLOTS; i++){
                    Slot& slot = m_pq_slots[(m_slot_ptr + i) % NUMBER_SLOTS];
                    if (slot.start_time <= event.time_us && event.time_us < slot.end_time){
                        slot.pq.push(event); break;
                    }
                }
            }

            std::optional<T> first_event(){
                return slots[m_slot_ptr].pq.top();
            }

            uint32_t size(){
                uin32_t size_ = 0;

                for (uin32_t i = 0; i < SLOTS, i++){
                    size_ += slots[i].pq.size();
                }

                return size_;
            }

            uint32_t fire(int64_t time_ns){
                uint32_t fired = 0;

                for (int i = 0; i < SLOTS; i++){
                    auto &pq = m_pq_slots[m_slot_ptr].pq;
                    while (!pq.empty() && pq.top().t <= new_time_us){
                        Timed t = pq.top(); pq.pop();
                        me()->fire(t.value, t.t);
                        ++fired;
                    }
                    
                    if (new_time_us < m_pq_slots[m_slot_ptr].end_time) break;
                
                    m_slots[m_slot_ptr].start_time += SLOTS * INTERVAL;
                    m_slots[m_slot_ptr].end_time += SLOTS * INTERVAL;
                    m_slot_ptr = (m_slot_ptr + 1) % SLOTS;
                }

                return fired;
            }

            void move(int64_t time_ns){
                // this is to pour values from far to mid, and from mid to near
                for (int i = 0; i < SLOTS; i++){
                    auto &pq = m_pq_slots[m_slot_ptr].pq;
                    while (!pq.empty() && pq.top().t <= new_time_us){
                        Timed t = pq.top(); pq.pop();
                        me()->move(t.value, t.t);
                    }
                    
                    if (new_time_us < m_pq_slots[m_slot_ptr].end_time) break;
                
                    m_slots[m_slot_ptr].start_time += SLOTS * INTERVAL;
                    m_slots[m_slot_ptr].end_time += SLOTS * INTERVAL;
                    m_slot_ptr = (m_slot_ptr + 1) % SLOTS;
                }
            }

        private:
            struct Timed{
                T value;
                uint64_t t;
            }    

            struct Compare {
                bool operator()(const Timed& a, const Timed& b) const {
                    return a.t > b.t; 
                }
            };

            struct Slot{
                uint64_t start_time;
                uint64_t end_time;
                std::priority_queue<Timed, std::vector<Timed>, Comapre> pq;
            }

            uint64_t m_curr_time_ns;
            uin32_t m_slot_ptr;
            Slot m_slots[SLOTS];
    };
}