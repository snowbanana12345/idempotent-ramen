#include <queue>
#include <unordered_map>
#include <functional>
#include <iostream>

namespace hftu{
    template <typename T, uint32_t SLOTS, int64_t INTERVAL>
    class SlotMapHeaps{
        using CallBack = std::function<void(T, int64_t)>;
        using Predicate = std::function<bool(T, int64_t)>;

        public:
            SlotMapHeaps(){
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
                if (time_ns < start_time() || time_ns >= end_time()) return;
                uint32_t offset = (time_ns - start_time()) / INTERVAL;
                uint32_t ind = (m_slot_ptr + offset) % SLOTS;
                m_slots[ind].pq.push({value, time_ns});
                m_slots[ind].time_map.emplace(value, time_ns);
            }

            bool remove(T value){
                uint32_t removed = 0;
                for (uint32_t i = 0; i < SLOTS; i++){
                    auto& pq = m_slots[i].pq;
                    auto& mp = m_slots[i].time_map;
                    removed += mp.erase(value);
                    while (!pq.empty() && mp[pq.top().value] != pq.top().t){
                        pq.pop();
                    }
                }

                return removed > 0;
            }

            int64_t first_event_time() const{
                for (int i = 0; i < SLOTS; i++){
                    uint32_t ind = (m_slot_ptr + i) % SLOTS;
                    auto& pq = m_slots[ind].pq;
                    if (!pq.empty()) return pq.top().t;
                }

                return INT64_MAX;
            }

            uint32_t size() const{
                uint32_t size_ = 0;

                for (uint32_t i = 0; i < SLOTS; i++){
                    size_ += m_slots[i].time_map.size();
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
                std::unordered_map<T, uint64_t> time_map;
            };

            int64_t m_curr_time;
            uint32_t m_slot_ptr;
            Slot m_slots[SLOTS];

            uint32_t fire_all(int64_t time_ns, CallBack call_back){
                uint32_t fired = this->size();
                for (int i = 0; i < SLOTS; i++) {
                    auto &pq = m_slots[m_slot_ptr].pq;
                    auto &mp = m_slots[m_slot_ptr].time_map;
                    while (!pq.empty()){
                        Timed t = pq.top(); pq.pop();
                        if(mp[t.value] == t.t) call_back(t.value, t.t);
                    }
                    
                    fired += mp.size();
                    m_slot_ptr = (m_slot_ptr + 1) % SLOTS;
                    mp.clear();
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
                    auto &mp = m_slots[m_slot_ptr].time_map;
                    if (time_ns < m_slots[m_slot_ptr].end_time){
                        while (!pq.empty() && pq.top().t <= time_ns){
                            Timed t = pq.top(); pq.pop();
                            auto it = mp.find(t.value);
                            if (it != mp.end() && it->second == t.t){
                                call_back(t.value, t.t);
                                mp.erase(it);
                            }
                            
                            ++fired;
                        }
                        break;
                    }
                    else { // process the entire block
                        while (!pq.empty()){
                            Timed t = pq.top(); pq.pop();
                            if(mp[t.value] == t.t) call_back(t.value, t.t);
                        }
                        fired += mp.size();
                        mp.clear();

                        m_slots[m_slot_ptr].start_time += SLOTS * INTERVAL;
                        m_slots[m_slot_ptr].end_time += SLOTS * INTERVAL;
                        m_slot_ptr = (m_slot_ptr + 1) % SLOTS;
                    }
                }

                return fired;
            }
    };
}