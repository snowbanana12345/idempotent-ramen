#include <queue>

namespace hftu{
    template <typename T, typename Derived, uint32_t SLOTS, uint64_t INTERVAL>
    class SlotHeaps{
        public:
            enum Op{
                FIRE,
                MOVE
            };

            SlotHeaps(){
                m_curr_time = 0;
                m_slot_ptr = 0;
                for (uint32_t i = 0; i < SLOTS; i++){
                    m_slots[i].start_time = m_curr_time + i * INTERVAL;
                    m_slots[i].end_time = m_curr_time + (i + 1) * INTERVAL;
                }
            }

            Derived* me() { return static_cast<Derived*>(this); }

            uint64_t start_time(){
                return m_slots[m_slot_ptr].start_time;
            }
            uint64_t end_time(){
                return start_time() + SLOTS * INTERVAL;
            }

            void insert(T value, uint64_t time_ns){
                for (int i = 0; i < SLOTS; i++){
                    Slot& slot = m_slots[(m_slot_ptr + i) % SLOTS];
                    if (slot.start_time <= time_ns && time_ns < slot.end_time){
                        slot.pq.push({value, time_ns}); break;
                    }
                }
            }

            int64_t first_event_time(){
                if (m_slots[m_slot_ptr].pq.empty()) return INT64_MAX;
                return m_slots[m_slot_ptr].pq.top().t;
            }

            uint32_t size(){
                uint32_t size_ = 0;

                for (uint32_t i = 0; i < SLOTS; i++){
                    size_ += m_slots[i].pq.size();
                }

                return size_;
            }

            uint32_t advance(uint64_t time_ns){
                return advance(time_ns, Op::FIRE);
            }

            uint32_t advance(uint64_t time_ns, Op op){
                uint32_t fired = 0;

                while (true) {
                    auto &pq = m_slots[m_slot_ptr].pq;
                    while (!pq.empty() && pq.top().t <= time_ns){
                        Timed t = pq.top(); pq.pop();

                        switch (op){
                            case Op::FIRE : me()->fire(t.value, t.t);
                            case Op::MOVE : me()->move(t.value, t.t);
                        }
                        
                        ++fired;
                    }
                    
                    if (time_ns < m_slots[m_slot_ptr].end_time) break;
                
                    m_slots[m_slot_ptr].start_time += SLOTS * INTERVAL;
                    m_slots[m_slot_ptr].end_time += SLOTS * INTERVAL;
                    m_slot_ptr = (m_slot_ptr + 1) % SLOTS;
                }

                return fired;
            }

        private:
            struct Timed{
                T value;
                uint64_t t;
            };    

            struct Compare {
                bool operator()(const Timed& a, const Timed& b) const {
                    return a.t > b.t; 
                }
            };

            struct Slot{
                uint64_t start_time;
                uint64_t end_time;
                std::priority_queue<Timed, std::vector<Timed>, Compare> pq;
            };

            uint64_t m_curr_time;
            uint32_t m_slot_ptr;
            Slot m_slots[SLOTS];
    };
}