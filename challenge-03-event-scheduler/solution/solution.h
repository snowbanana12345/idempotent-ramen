#pragma once
// Challenge 03: Event Scheduler — Naive Reference Implementation
// This is correct but slow. You can do much better!

#include "../scheduler.h"
#include <map>
#include <unordered_map>
#include <climits>

// Your EventScheduler must support:
//
//   schedule(event_id, time_us)
//     Schedule a one-shot event at the given absolute time (microseconds).
//     If event_id is already scheduled, replace it (implicit cancel + reschedule).
//
//   cancel(event_id) -> bool
//     Cancel a pending event. Returns true if found and cancelled.
//
//   advance(new_time_us, callback, user_data) -> uint32_t
//     Advance the clock to new_time_us. Fire all events with time <= new_time_us
//     by calling callback(event_id, scheduled_time, user_data).
//     Time always advances monotonically (new_time_us >= previous new_time_us).
//     Events at the same microsecond may fire in any order.
//     Returns number of events fired.
//
//   size() -> uint64_t
//     Number of pending events.
//
//   next_event_time() -> int64_t
//     Time of next event, or INT64_MAX if empty.
//

// #include "default_impl.h"
// #include "heap_impl.h"
// #include "micro_bucket_impl.h"
// #include "slot_heap_impl.h"
#include "micro_hashmap_impl.h"