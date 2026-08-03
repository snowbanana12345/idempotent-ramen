## introduction

We have a sorted set problem, we can try to optimize it
- Near, mid, and far handling
- Batch efficiency, ideally the entire batch could be loaded into cache in one go.



## default solution performance

  Latency (cycles): p50=42  p99=750  p999=1292  max=25465993  avg=318
  Latency (cycles): p50=42  p99=666  p999=1041  max=24681870  avg=301
  Latency (cycles): p50=42  p99=667  p999=1125  max=29425529  avg=338
{
  "benchmarks": [
    {
      "name": "BM_Solution",
      "iterations": 7,
      "ops_per_iteration": 100000,
      "total_cycles": 933800000,
      "cycles_per_op": 1334.00
    }
  ]
}

## std::priority_queue implementation

  Latency (cycles): p50=41  p99=500  p999=792  max=34009916  avg=306
  Latency (cycles): p50=41  p99=417  p999=666  max=26081904  avg=249
  Latency (cycles): p50=41  p99=459  p999=750  max=29702520  avg=252
{
  "benchmarks": [
    {
      "name": "BM_Solution",
      "iterations": 11,
      "ops_per_iteration": 100000,
      "total_cycles": 1009800000,
      "cycles_per_op": 918.00
    }
  ]
}

Better than the default implementation.
std::multi_map is implementated as nodes, it has terrible cache behavior as the nodes of the tree are scattered
all over memory. std::priority_queue has better performance as its a vector underneath. But it still has bad cache
behavior when its large as the bubble up algo had to jump through the entire array.

## tiered std::priority_queue implementation
Idea is that we split the data into near, mid, and far. This reduces the size of each array we have to go through.

  Latency (cycles): p50=41  p99=417  p999=792  max=70393761  avg=484
  Latency (cycles): p50=41  p99=458  p999=750  max=34268907  avg=300
  Latency (cycles): p50=41  p99=458  p999=667  max=32791647  avg=290
{
  "benchmarks": [
    {
      "name": "BM_Solution",
      "iterations": 10,
      "ops_per_iteration": 100000,
      "total_cycles": 916000000,
      "cycles_per_op": 916.00
    }
  ]
}

This actually made the p99 slightly better. It certainly made the average worse because we need to
pour the mid to the near and far to the mid which costs computation.

## micro second bucket ring buffer

Since the nearest 1000 us are hot, we create a 1,000 X 2,000 buffer
the first 1000 repesents we want to have 1 slot for each microsecond.
the 2,000 is the guess that we take the vast majority of the time, there will not be more than 2000 at a time
we also create an overflow buffer that is a priority_queue incase a microsecond slot overflows.
Hopefully this overflow buffer ends up small enough that it fits into cache
To avoid having to dynamically allocate memory, we use a ring buffer and just keep advancing around it.

