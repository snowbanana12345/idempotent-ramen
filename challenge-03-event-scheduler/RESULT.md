## default solution performance
"cycles_per_op": 1334.00
baseline implementation

## std::priority_queue implementation
"cycles_per_op": 1000.00
see files : heap_impl.h

performs better than default implentation
priority_queue for ordering.
hashmap for invalidation checks.

## micro second bucket ring buffer
"cycles_per_op": 5168.00
See files : micro_bucket_impl.h

store near events in 2D array.
Sequentially polling events is very effcient for batch processing
Store events that exceed the array's capacity in a overflow buffer implemented with std::priority_queue
Mid and Far events are stored using priority_queues and moved into the bucket array during advance

Moving events and checking overflow buffers costs performance.
Empty loops through the buckets can cost performance.

## micro second bucket ring buffer with hash maps
"cycles_per_op": 2750.00
see files : micro_hashmap_impl.h

we replace each of the microsecond slots with hashmaps
The hashmaps can check for 
Ordering within a microsecond is not important.

May not be production viable.
After running for long time, each of hashmap could be resized to maximum number of events
Using a overflow priority_queue costs performance

** why microsecond bucket doesn't work well **

The key assumption as to why we expect this implementation to perform is that is very efficient in batch processing
Remember, the order of processed events within the same microsecond is unimportant.
This implementation seeks to leverage that. 

We count the number of calls to empty buckets
empty map counter : 39938572 ~= 30 million
advance() is called 120k times.
Average 300 empty loops / call

Most microsecond buckets are empty at a given time.

## slotted std::priority_queue
"cycles_per_op": 1250.00
see files : slot_heap_impl.h , slot_heap.h

instead of O(log n) where n is size of whole data size.
Only the closest events will be actively accessed 

Stale records can pile up below top element of priority_queue.
Causes excess memory allocations in production.

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