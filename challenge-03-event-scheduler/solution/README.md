## introduction

We have a sorted set problem, we can try to optimize it
- Near, mid, and far handling
- Batch efficiency, ideally the entire batch could be loaded into cache in one go.


## default solution performance

------- Latency (cycles) by operation -------- 
  Schedule: p50=83  p99=750  p999=1208  max=9375  avg=155 n=69761
  Cancel:   p50=0  p99=500  p999=833  max=7916  avg=47 n=39915
  Advance:  p50=208  p99=792  p999=152088  max=25654034  avg=1250 n=40100
  QuerySz:  p50=0  p99=125  p999=375  max=3333  avg=9 n=30311
  QueryNext:p50=0  p99=125  p999=250  max=792  avg=11 n=19913
    All:    p50=42  p99=708  p999=1250  max=25654034  avg=317 n=200000

The bulk of the p99 latency comes from the Schedule and Advance methods.
Especially the advance method which could potentially trigger 

## std::priority_queue implementation

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

## slotted tiered std::priority_queue
Lets take the tiered std::priority_queue idea. It appeared to have reduced the p99 just a little bit.
We put the near events into slots of priority queues


------- Latency (cycles) by operation -------- 
  Schedule: p50=42  p99=250  p999=375  max=6292  avg=54 n=69761
  Cancel:   p50=42  p99=291  p999=375  max=625  avg=61 n=39915
  Advance:  p50=167  p99=459  p999=43350  max=40598604  avg=1306 n=40100
  QuerySz:  p50=0  p99=42  p999=208  max=292  avg=8 n=30311
  QueryNext:p50=0  p99=125  p999=291  max=625  avg=16 n=19913
    All:    p50=42  p99=375  p999=542  max=40598604  avg=295 n=200000