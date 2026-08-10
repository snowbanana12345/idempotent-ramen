#include "base.h"


namespace hftu{
    /*
    
    Since the nearest 1000 us are hot, we create a 1,000 X 1,000 buffer
    the first 1000 repesents we want to have 1 slot for each microsecond.
    the 1,000 is the guess that we take the vast majority of the time, there will not be more than 1000 at a time
    we also create an overflow buffer that is a priority_queue incase a microsecond slot overflows.
    Hopefully this overflow buffer ends up small enough that it fits into cache
    To avoid having to dynamically allocate memory, we use a ring buffer and just keep advancing around it.

    ----- void schedule(uint64_t event_id, int64_t time_us) ------
    if its within 1 millisecond, we put it into the micro buffer (O(1) time)
    if its within 1 second, we put it into the mid priority_queue buffer (O(log n) time)
    if its beyond 1 second, we put it into the far priority_queue buffer (O(log n) time)

    the micro buffer is a ring buffer, m_slot_ptr timepoint = m_curr_time_us
    if time_us is within 1 millisecond, we put it into the slot at (time_us - m_curr_time_us) % MICRO_SLOTS
    if more than SLOT_SIZE number of events squeezes into the same microsecond slot, we put the overflow into the overflow buffer, which is a priority_queue

    we store the event_id -> time_us mapping in a hash table

    This is to handle cancel and reschedule
    On reschedule, we do not attempt to scan the micro buffer to remove the old event
    We just update the time and check on read out if the entry is still valid

    ------ bool cancel(uint64_t event_id) ------
    We delete the event_id from the hash table, and return true if it was found
    if any of the buffers have events that are no longer valid, we pop them off the buffer
    Most of the time, this will not result is an optimization, but if might split off just a little
    bit of work from the advance() function, which is the hot path

    ------ uint32_t advance(int64_t new_time_us, EventCallback cb, void* user_data) ------

    we first loop through the micro buffer microsecond by microsecond, and fire all events that are valid
    for each micro timestamp, we then check the overflow buffer, and fire all events that are valid and have time_us
    This satisfies correctness for the ordering of events fired

    We check the mid and far priority queues to handle the edge case that advance goes past 1000 us in one call

    finally, we move the events buffers since the time advanced. mid -> near, far -> mid

    This is where the micro buffer is much more efficient than a priority queue storing all events.
    The events spanning several microseconds can be fitted into cache. 
    Events spanning longer does not have to constantly perform bubbling operations that causes cache misses
    The overflow buffer is tuned to be smaller enough that it fits into cache

    Mid and far buffers are still O(log n)
    but the bulk of the performance cost in advancing long times is absorbed by the micro buffer

    - uint64_t size() const
    O(1) time, just return the size of the hash table

    int64_t next_event_time() const;
    O(1000) time, we check the micro buffer first, 
    then the mid buffer, then the far buffer in O(1) time

    */

    constexpr uint32_t MICRO_SLOTS = 1000;
    constexpr uint32_t SLOT_SIZE = 1000;
    constexpr int64_t FAR_THRESHOLD = 1'000'000;

    class EventScheduler {
        public:
            EventScheduler (){
                std::memset(m_inner_ptrs, 0, sizeof(m_inner_ptrs));
                m_curr_time_us = 0;
                m_slot_ptr = 0;

                for (int i = 0; i < MICRO_SLOTS; ++i) {
                    std::memset(m_slots[i], 0, sizeof(m_slots[i]));
                }
            }

            void schedule(uint64_t event_id, int64_t time_us){
                if(time_us - m_curr_time_us < MICRO_SLOTS){
                    insert_into_slots(event_id, time_us);
                } else if(time_us - m_curr_time_us < FAR_THRESHOLD){
                    m_mid_pq.push({event_id, time_us});
                } else {
                    m_far_pq.push({event_id, time_us});
                }

                m_events[event_id] = time_us;
                pop_invalid_buffer_heads();
            }

            bool cancel(uint64_t event_id){
                uint64_t deleted = m_events.erase(event_id) > 0;
                pop_invalid_buffer_heads();
                return deleted;
            }

            uint32_t advance(int64_t new_time_us, EventCallback cb, void* user_data){
                uint32_t fired_count = 0;
                for (uint32_t i = 0; i < MICRO_SLOTS; ++i) {
                    int64_t slot_time_us = m_curr_time_us + i;
                    std::cout << m_slot_ptr << " " << slot_time_us << " " << i << " " << m_curr_time_us << std::endl;
                    
                    for (uint32_t j = 0; j < m_inner_ptrs[m_slot_ptr]; ++j) {
                        uint64_t event_id = m_slots[m_slot_ptr][j];
                        if (is_event_valid({event_id, slot_time_us})) {
                            cb(event_id, slot_time_us, user_data);
                            fired_count++;
                            m_events.erase(event_id);
                        }
                    }

                    while (!m_overflow_buffer.empty() && m_overflow_buffer.top().time_us <= slot_time_us) {
                        Event event = m_overflow_buffer.top();
                        m_overflow_buffer.pop();
                        if (is_event_valid(event)) {
                            cb(event.event_id, event.time_us, user_data);
                            fired_count++;
                            m_events.erase(event.event_id);
                        }
                    }

                    if (slot_time_us >= new_time_us) break;
                    m_inner_ptrs[m_slot_ptr] = 0;
                    m_slot_ptr = (m_slot_ptr + 1) % MICRO_SLOTS; 
                }

                m_curr_time_us = new_time_us;
                
                // ---- correctness for the case of new_time_us jumping milliseconds -----
                while (!m_mid_pq.empty() && m_mid_pq.top().time_us <= new_time_us) {
                    Event e = m_mid_pq.top();
                    m_mid_pq.pop();
                    if (is_event_valid(e)) {
                        cb(e.event_id, e.time_us, user_data);
                        fired_count++;
                        m_events.erase(e.event_id);
                    }
                }

                while (!m_far_pq.empty() && m_far_pq.top().time_us <= new_time_us) {
                    Event e = m_far_pq.top();
                    m_far_pq.pop();
                    if (is_event_valid(e)) {
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

                // ---- the pop operations do not gaurantee that the heads of the buffers are valid, so we pop them off ----

                pop_invalid_buffer_heads();

                return fired_count;
            }

            uint64_t size() const{
                return m_events.size();
            }
            
            int64_t next_event_time() const{
                // m_inner_ptrs array can be loaded into cache
                // do not need to check overflow buffer
                // the slot must contain at least one element for the overflow buffer 
                // to contain an event that is earlier than the next event in the slots
                // for the micro buffer we have to check if each slot is still valid
                // for the mid and far buffers, the invariant that the head is valid is maintained by the other function calls
                for (uint32_t i = 0; i < MICRO_SLOTS; ++i) {
                    uint32_t slot_index = (m_slot_ptr + i) % MICRO_SLOTS;
                    for (uint32_t j = 0; j < m_inner_ptrs[slot_index]; ++j) {
                        uint64_t event_id = m_slots[slot_index][j];
                        if (is_event_valid({event_id, m_curr_time_us + i})) {
                            return m_curr_time_us + i;
                        }
                    }
                }

                if (!m_mid_pq.empty()) {
                    return m_mid_pq.top().time_us;
                }

                if (!m_far_pq.empty()) {
                    return m_far_pq.top().time_us;
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
            uint32_t m_slot_ptr = 0;
            uint32_t m_inner_ptrs[MICRO_SLOTS];

            int64_t m_curr_time_us = 0;

            std::unordered_map<uint64_t, int64_t> m_events;
            std::priority_queue<Event, std::vector<Event>, EventComparator> m_overflow_buffer;
            std::priority_queue<Event, std::vector<Event>, EventComparator> m_mid_pq;
            std::priority_queue<Event, std::vector<Event>, EventComparator> m_far_pq;

            void insert_into_slots(uint64_t event_id, int64_t time_us){
                uint32_t offset = static_cast<uint32_t>(time_us - m_curr_time_us);
                uint32_t slot_index = (m_slot_ptr + offset) % MICRO_SLOTS;
                uint32_t inner_index = m_inner_ptrs[slot_index];
                if (inner_index >= SLOT_SIZE) { // this branch should only run < 1% of the time
                    m_overflow_buffer.push({event_id, time_us});
                } else { // this should be the majority branch
                    m_slots[slot_index][inner_index] = event_id;
                    m_inner_ptrs[slot_index]++;
                }
            }

            inline bool is_event_valid(Event event) const {
                // this boolean condition very likely cannot be predicted properly by the CPU branch predictor
                auto it = m_events.find(event.event_id);
                return it != m_events.end() && it->second == event.time_us;
            }

            void pop_invalid_buffer_heads(){
                while (!m_overflow_buffer.empty() && !is_event_valid(m_overflow_buffer.top())) {
                    m_overflow_buffer.pop();
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