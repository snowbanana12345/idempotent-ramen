## default solution performance
"cycles_per_op": 1334.00
baseline implementation

## std::priority_queue implementation
"cycles_per_op": 1000.00
see files : heap_impl.h

performs better than default implentation
priority_queue for ordering.
hashmap for invalidation checks.

## slotted priority_queue


## micro second bucket ring buffer
See files : micro_bucket_impl.h


## micro second bucket ring buffer with hash maps

see files : micro_hashmap_impl.h

we replace each of the microsecond slots with hashmaps
This makes it better because the hashmaps themselves can check if a event has been cancelled.
This avoids maintaining the overflow priority_queue and checking it on every loop

------- Latency (cycles) by operation -------- 
Schedule: p50=42  p99=208  p999=333  max=10726  avg=46 n=209283
Cancel:   p50=0  p99=125  p999=250  max=375  avg=17 n=119745
Advance:  p50=1042  p99=1542  p999=226057  max=34165145  avg=2400 n=120300
QuerySz:  p50=0  p99=42  p999=84  max=7583  avg=7 n=90933
QueryNext:p50=84  p99=375  p999=500  max=2083  avg=122 n=59739
  All:    p50=42  p99=1375  p999=1666  max=34165145  avg=514 n=600000
"cycles_per_op": 2750.00

The key assumption as to why we expect this implementation to perform is that is very efficient in batch processing
Remember, the order of processed events within the same microsecond is unimportant.
This implementation seeks to leverage that. 
We count the number of empty loops into empty buckets.

empty map counter : 39938572 ~= 30 million
from the above stats, the advance method is called 120k times.
On every advance call, on average, our implementation hits 300 empty buckets.

This should explain why the micro bucket idea doesn't quite work for this workload.

## slotted std::priority_queue

see files : slot_heap_impl.h , slot_heap.h

Idea is to divide the interval 0 to 1000 us into slots
Each slot has its own priority_queue
On advance, the bubble up algo will only apply to the priority_queue in the first slot
this achieves sub O(n log n) performance on advance
The slots are arranged in a rotating ring to avoid dynamically reallocating the queues

------- Latency (cycles) by operation -------- 
Schedule: p50=84  p99=250  p999=334  max=64577  avg=105 n=558088
Cancel:   p50=83  p99=291  p999=375  max=8375  avg=94 n=319320
Advance:  p50=375  p99=750  p999=150671  max=34689111  avg=1457 n=320800
QuerySz:  p50=41  p99=42  p999=58  max=4958  avg=23 n=242488
QueryNext:p50=0  p99=42  p999=42  max=125  avg=10 n=159304
All:    p50=83  p99=625  p999=833  max=34689111  avg=352 n=1600000
"cycles_per_op": 1250.00

It's not better actually. 
To handle reschedule and advance, it needs to constantly check against a large hashmap for membership and removal
Large hashmap access is non cache friendly as the event_id are randomly scattered throughout the hashmap

## slotted heap map
"cycles_per_op": 1334.00
see file : slot_heap_map_impl.h slot_heap_map.h

std::priority_queue + std::unordered_map implementation
priority_queue keeps order, map keeps track of deletions
break up periods between near, mid, far.
In each near, mid, far, break up each of 
improve cache performance as advance hits a priority queue containing only a fraction of total data
extra performance cost due to shifting from far->mid, mid->near

Production viable. Stale entries buried below the top element inside priority_queue will
be cleared when time advances past them.