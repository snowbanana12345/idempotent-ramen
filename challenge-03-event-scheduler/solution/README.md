## introduction

We have a sorted set problem, we can try to optimize it
- Near, mid, and far handling
- Batch efficiency, ideally the entire batch could be loaded into cache in one go.


## default solution performance

base line correct solution that is provided.
give O(log n) performance. 
Cache performance is bad because tree structures with nodes have their memory dispersed
They have to constantly dynamically allocate memory on the heap on insert and delete operations

------- Latency (cycles) by operation -------- 
  Schedule: p50=83  p99=750  p999=1208  max=9375  avg=155 n=69761
  Cancel:   p50=0  p99=500  p999=833  max=7916  avg=47 n=39915
  Advance:  p50=208  p99=792  p999=152088  max=25654034  avg=1250 n=40100
  QuerySz:  p50=0  p99=125  p999=375  max=3333  avg=9 n=30311
  QueryNext:p50=0  p99=125  p999=250  max=792  avg=11 n=19913
    All:    p50=42  p99=708  p999=1250  max=25654034  avg=317 n=200000
"cycles_per_op": 1334.00

The bulk of the p99 latency comes from the Schedule and Advance methods.
Especially the advance method which could potentially trigger 

## std::priority_queue implementation

see files : heap_impl.h

use an std::priority_queue to determine ordering
A hashmap as a source of truth for actual events.
Events popped out of the priority_queue might be invalid so need to check against the 

------- Latency (cycles) by operation -------- 
Schedule: p50=42  p99=167  p999=292  max=10333  avg=38 n=69761
Cancel:   p50=0  p99=125  p999=209  max=3542  avg=16 n=39915
Advance:  p50=208  p99=667  p999=155796  max=34112569  avg=1449 n=40100
QuerySz:  p50=0  p99=42  p999=125  max=917  avg=7 n=30311
QueryNext:p50=0  p99=42  p999=125  max=834  avg=9 n=19913
  All:    p50=41  p99=500  p999=833  max=34112569  avg=309 n=200000
"cycles_per_op": 1000.00

Better than the default implementation.
std::multi_map is implementated as nodes, it has terrible cache behavior as the nodes of the tree are scattered
all over memory. std::priority_queue has better performance as its a vector underneath. But it still has bad cache
behavior when its large as the bubble up algo had to jump through the entire array.

## tiered std::priority_queue implementation
Idea is that we split the data into near, mid, and far. This reduces the size of each array we have to go through.
This actually made the p99 slightly better. It certainly made the average worse because we need to
pour the mid to the near and far to the mid which costs computation.
But its still not good enough

## micro second bucket ring buffer

See files : micro_bucket_impl.h

Since the nearest 1000 us are hot, we create a 1,000 X 1,000 buffer
the first 1000 repesents we want to have 1 slot for each microsecond.
the 1,000 is the guess that we take the vast majority of the time, there will not be more than 1000 at a time
we also create an overflow buffer that is a priority_queue incase a microsecond slot overflows.
Hopefully this overflow buffer ends up small enough that it fits into cache
To avoid having to dynamically allocate memory, we use a ring buffer and just keep advancing around it.

rescheduling and cancels are handled by maintaining a hash map

------- Latency (cycles) by operation -------- 
  Schedule: p50=42  p99=334  p999=958  max=8518  avg=56 n=69761
  Cancel:   p50=0  p99=250  p999=875  max=6250  avg=31 n=39915
  Advance:  p50=2292  p99=3667  p999=125695  max=45695082  avg=3829 n=40100
  QuerySz:  p50=0  p99=125  p999=542  max=3392  avg=10 n=30311
  QueryNext:p50=125  p99=583  p999=791  max=5500  avg=163 n=19913
    All:    p50=42  p99=2750  p999=5334  max=45695082  avg=811 n=200000

Oh my Ramen the results are bad.

Lets try to do some fake optimizations like removing 
- the validity check against the events stored in the hashmap
- removing the over flow buffer. This will remove a lot of the conditional checks inside the loop
This will violate correctness but we are just trying to debug what is causing performance issues
The results are below.

------- Latency (cycles) by operation -------- 
  Schedule: p50=83  p99=792  p999=4125  max=7762882  avg=294 n=69761
  Cancel:   p50=42  p99=584  p999=875  max=23434  avg=112 n=39915
  Advance:  p50=2083  p99=2875  p999=11708  max=14460957  avg=2557 n=40100
  QuerySz:  p50=0  p99=42  p999=209  max=1042  avg=7 n=30311
  QueryNext:p50=209  p99=708  p999=959  max=6560  avg=226 n=19913
    All:    p50=84  p99=2417  p999=7559  max=14460957  avg=661 n=200000

There's some improvement, but it looks like having to loop through each slot is very slow
I'm going to say this microsecond bucket idea is ramen eggs.

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

The idea is to break up the hash maps into slots
Instead of a huge global hash map
When advancing, each call of the advance would be accessing a much smaller hash map
Also, when advancing past an entire slot, instead of individually deleting each hash map key,
we can call map.clear() at the end which improves batch efficiency.

------- Latency (cycles) by operation -------- 
Schedule: p50=125  p99=334  p999=3958  max=24685  avg=138 n=348805
Cancel:   p50=84  p99=333  p999=3375  max=33702  avg=117 n=199575
Advance:  p50=375  p99=1417  p999=240289  max=32422477  avg=1747 n=200500
QuerySz:  p50=0  p99=84  p999=208  max=11767  avg=10 n=151555
QueryNext:p50=41  p99=167  p999=292  max=8750  avg=28 n=99565
All:    p50=125  p99=834  p999=6102  max=32422477  avg=426 n=1000000
"cycles_per_op": 1668.00

This did not make it better at all.
Breaking the hashmap into smaller hashmaps required more overhead on our part to route the orders into the correct hashmap
The improvement in hashmap access if any does not cover that.