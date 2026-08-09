## Introduction

Easier version of the event scheduler that has to support cancel and reschedule.
The data distribution is the same

## default implementation
Latency (cycles): p50=0  p99=167  p999=3625  max=3110459  avg=73
Latency (cycles): p50=0  p99=167  p999=4166  max=3363601  avg=75
Latency (cycles): p50=0  p99=167  p999=3458  max=3614365  avg=73
"cycles_per_op": 334.00

## heap implementation
  Latency (cycles): p50=0  p99=167  p999=6958  max=7323761  avg=156
  Latency (cycles): p50=0  p99=125  p999=5625  max=5711912  avg=121
  Latency (cycles): p50=0  p99=166  p999=5417  max=5656752  avg=111
"cycles_per_op": 332.00



## slotted heap implementation



Best run so far.

  Latency (cycles): p50=42  p99=125  p999=6834  max=26042886  avg=230
  Latency (cycles): p50=42  p99=125  p999=6167  max=18885590  avg=187
  Latency (cycles): p50=42  p99=125  p999=6292  max=22677143  avg=194
  "name": "BM_Solution",
  "iterations": 40,
  "ops_per_iteration": 100000,
  "total_cycles": 1000000000,
  "cycles_per_op": 250.00

Parameters are

constexpr uint32_t NEAR_SLOTS = 10;
constexpr int64_t NEAR_INTERVAL = 100; // 0.1 us
constexpr int64_t NEAR_THRESHOLD = NEAR_SLOTS * NEAR_INTERVAL;

constexpr uint32_t MID_SLOTS = 10;
constexpr int64_t MID_INTERVAL = 100'000'000; // 100ms
constexpr int64_t MID_THRESHOLD = MID_SLOTS * MID_INTERVAL;

constexpr uint32_t FAR_SLOTS = 60;
constexpr int64_t FAR_INTERVAL = 1'000'000'000; // 1 second

for the first 1us, divide into 10 slots of 100 ns each
for the next 1 second, divide into 10 slots of 100ms each
for the next 1 min, divide into 60 slots of 1s each.


