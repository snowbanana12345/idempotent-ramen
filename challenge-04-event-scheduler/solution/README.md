## Introduction

Easier version of the event scheduler that has to support cancel and reschedule.
The data distribution is the same

## default implementation
default correct implementation provided


Latency (cycles): p50=0  p99=167  p999=3625  max=3110459  avg=73
Latency (cycles): p50=0  p99=167  p999=4166  max=3363601  avg=75
Latency (cycles): p50=0  p99=167  p999=3458  max=3614365  avg=73
"cycles_per_op": 334.00

## heap implementation

see files : heap_impl.h

Both the default implementation and using std::priority_queue stores the 
entire dataset into a single sorted data structure.
This is very unnecessary as the advance iterates through the head of the dataset
although the heap is stored in a continuous array, the bubble sort algo moves elements from one
end to the other which is cache unfriendly when the array is large

Latency (cycles): p50=0  p99=167  p999=6958  max=7323761  avg=156
Latency (cycles): p50=0  p99=125  p999=5625  max=5711912  avg=121
Latency (cycles): p50=0  p99=166  p999=5417  max=5656752  avg=111
"cycles_per_op": 332.00


## slotted heap implementation

see files : slot_heap_impl.h, slot_heap.h

We attempt to find an improvement by breaking up sorted sets into time_ranges.
slot1, t = 100 to t = 199. We use a rotating ring buffer to recycle the priority_queues as the time advances.
Further, we break up the dataset into time periods to isolate the nearest scheduled objects (hot store) from the further objects (cold store)
During batch processing, advance(1 millisecond) which is the cause of a high p99 latency,
our  batch performance is somewhere between O(n) and O(log n) where n is only the number of events
in the hot store. We do bubble sort algo only over a fraction of the data set, improving cache friendliness of priority_queues
Note that increasing the number of slots reduces the bubble sort size, but introduces extra latency due to the logic
in managing the ring buffer.


------- results --------

Latency (cycles): p50=42  p99=125  p999=6834  max=26042886  avg=230
Latency (cycles): p50=42  p99=125  p999=6167  max=18885590  avg=187
Latency (cycles): p50=42  p99=125  p999=6292  max=22677143  avg=194
"name": "BM_Solution",
"iterations": 40,
"ops_per_iteration": 100000,
"total_cycles": 1000000000,
"cycles_per_op": 250.00

------ Parameters are ------
 
constexpr uint32_t NEAR_SLOTS = 16;
constexpr int64_t NEAR_INTERVAL = 64; // ~ 1us

constexpr int64_t MICRO_SECOND = 1024;
constexpr int64_t MILLI_SECOND = 1024 * 1024;
constexpr int64_t SECOND = 1024 * 1024 * 1024;

constexpr uint32_t MID_SLOTS = 16;
constexpr int64_t MID_INTERVAL = 64 * MILLI_SECOND; // 100ms

constexpr uint32_t FAR_SLOTS = 64;
constexpr int64_t FAR_INTERVAL = SECOND; // 60 second

for the first 1us, divide into 10 slots of 100 ns each
for the next 1 second, divide into 10 slots of 100ms each
for the next 1 min, divide into 60 slots of 1s each.


