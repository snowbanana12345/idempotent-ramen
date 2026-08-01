## introduction





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
behavior when its large as 

