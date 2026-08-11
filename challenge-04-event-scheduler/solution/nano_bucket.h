#include <vector>
#include <queue>

namespace hftu{
    template <typename T, uint32_t BUCKETS>
    class NanoBuckets{
        public:
            NanoBuckets(uint32_t initial_bucket_size){
                m_curr_time = 0;
                m_slot_ptr = 0;
                for (int i = 0; i < BUCKETS; i++){
                    m_buckets[i].reserve(initial_bucket_size);
                }
                m_size = 0;
            }

            int64_t start_time(){
                return m_curr_time;
            }   

            int64_t end_time(){
                return m_curr_time + BUCKETS;
            }

            void insert(T value, int64_t time_ns){
                if (time_ns < start_time() || time_ns >= end_time()) return;
                uint32_t offset = static_cast<uint32_t>(time_ns - m_curr_time);
                uint32_t ind = (m_slot_ptr + offset) % BUCKETS;
                if (m_buckets[ind].empty()) m_non_empty_times.push(time_ns);
                m_buckets[ind].push_back(value);
                m_size++;
            }

            uint32_t advance(uint64_t new_time_ns, std::function<void(T, int64_t)> call_back){
                uint32_t fired = 0;
                while (!m_non_empty_times.empty() && m_non_empty_times.top() <= new_time_ns){
                    int64_t t = m_non_empty_times.top(); 
                    m_non_empty_times.pop();
                    uint32_t offset = static_cast<uint32_t>(t - m_curr_time);
                    uint32_t ind = (m_slot_ptr + offset) % BUCKETS;
                    std::vector<T> &bucket = m_buckets[ind];
                    for (T value : bucket){
                        call_back(value, t);
                    }
                    fired += bucket.size();
                    m_size -= bucket.size();
                    bucket.clear();
                }
                
                uint32_t offset = static_cast<uint32_t>(new_time_ns - m_curr_time);
                m_slot_ptr = (m_slot_ptr + offset) % BUCKETS;
                m_curr_time = new_time_ns;

                return fired;
            }

            int64_t first_event_time() const{
                if (m_non_empty_times.empty()) return INT64_MAX;
                return m_non_empty_times.top();
            }

            uint32_t size() const{
                return m_size;
            }

        private:
            std::priority_queue<int64_t, std::vector<int64_t>, std::greater<int64_t>> m_non_empty_times;
            std::vector<T> m_buckets[BUCKETS];
            uint32_t m_slot_ptr;
            int64_t m_curr_time;
            uint32_t m_size;
    };
}