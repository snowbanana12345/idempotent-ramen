#include <vector>
#include <queue>
#include <boost/intrusive/slist.hpp>


namespace hftu{
    template <typename T, uint32_t BUCKETS, uint32_t SEGMENT_LENGTH>
    class LinkedNanoBuckets{
        private:
            class Segment : public boost::intrusive::slist_base_hook<> {
            public:
                Segment() = default;
                Segment(const Segment&) = delete;
                Segment(const Segment&&) = delete;
                ~Segment() = default;
                T data_[SEGMENT_LENGTH];
                uint32_t size_ = 0;
            };

        using LIST_TYPE =  boost::intrusive::slist<Segment>;

        public:
            LinkedNanoBuckets(uint32_t expected_elements){
                m_curr_time = 0;
                m_slot_ptr = 0;
                m_size = 0;

                uint32_t initial_segments = expected_elements / SEGMENT_LENGTH + 1;
                m_segment_pool.reserve(initial_segments);
                for (int i = 0; i < initial_segments; i++){
                    m_segment_pool.push_back(new Segment());
                }
            }

            inline int64_t start_time(){
                return m_curr_time;
            }   

            inline int64_t end_time(){
                return m_curr_time + BUCKETS;
            }

            void insert(T value, int64_t time_ns){
                if (time_ns < start_time() || time_ns >= end_time()) return;
                uint32_t offset = static_cast<uint32_t>(time_ns - m_curr_time);
                uint32_t ind = (m_slot_ptr + offset) % BUCKETS;
                if (m_lists[ind].empty()) m_non_empty_times.push(time_ns);
            
                LIST_TYPE& list = m_lists[ind];
                Segment* seg;

                if (list.empty() || list.front().size_ >= SEGMENT_LENGTH){ // frequently mispredicted branch
                    // we can simply push from the front of the pool. ordering within a nano second is not important
                    if (m_segment_pool.empty()){ // should almost never happen.
                        seg = new Segment();
                    }
                    else {
                        seg = m_segment_pool.back();
                        m_segment_pool.pop_back();
                    }
                    list.push_front(*seg);
                }
                else {
                    seg = &list.front();
                }

                seg->data_[seg->size_++] = value;
                m_size++;
            }

            uint32_t advance(uint64_t new_time_ns, std::function<void(T, int64_t)> call_back){
                uint32_t fired = 0;
                while (!m_non_empty_times.empty() && m_non_empty_times.top() <= new_time_ns){
                    int64_t t = m_non_empty_times.top(); 
                    m_non_empty_times.pop();
                    uint32_t offset = static_cast<uint32_t>(t - m_curr_time);
                    uint32_t ind = (m_slot_ptr + offset) % BUCKETS;
                    LIST_TYPE &list = m_lists[ind];
                    for (Segment& segment : list){
                        for (uint32_t i = 0; i < segment.size_; i++){
                            call_back(segment.data_[i], t);
                        }
                        fired += segment.size_;
                        m_size -= segment.size_;
                        segment.size_ = 0;
                        m_segment_pool.push_back(&segment);
                    }
                    
                    list.clear();
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
            LIST_TYPE m_lists[BUCKETS];
            std::vector<Segment*> m_segment_pool;
            uint32_t m_slot_ptr;
            int64_t m_curr_time;
            uint32_t m_size;
    };
}