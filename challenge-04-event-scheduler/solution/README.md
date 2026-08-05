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

Unlike the 
