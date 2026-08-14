## Default implementation

Store the book ordered by price in a std::map binary tree.

---- AGGREGATE LATENCIES ----
QUEUE_POSITION Latency (cycles): p50=250  p99=1541  p999=1916  max=9834  avg=357
VOLUME_NEAR_BEST Latency (cycles): p50=42  p99=167  p999=458  max=1542  avg=44
TOP_LEVELS Latency (cycles): p50=42  p99=208  p999=375  max=1083  avg=55
BEST_ASK Latency (cycles): p50=0  p99=42  p999=167  max=1500  avg=8
BEST_BID Latency (cycles): p50=42  p99=167  p999=334  max=2601  avg=52
CANCEL_OUR Latency (cycles): p50=42  p99=125  p999=458  max=3667  avg=50
MODIFY_OUR Latency (cycles): p50=41  p99=125  p999=334  max=4583  avg=37
SEND_OUR Latency (cycles): p50=42  p99=143  p999=625  max=8000  avg=45
MODIFY Latency (cycles): p50=458  p99=1584  p999=2042  max=18042  avg=532
CANCEL Latency (cycles): p50=500  p99=1625  p999=2083  max=19851  avg=600
ADD Latency (cycles): p50=125  p99=500  p999=1000  max=16560  avg=160
---- END ----

"cycles_per_op": 311.69


## replacing std::list with std::vector

-------- AGGREGATE LATENCIES ----
QUEUE_POSITION Latency (cycles): p50=125  p99=250  p999=334  max=16518  avg=124
VOLUME_NEAR_BEST Latency (cycles): p50=42  p99=143  p999=375  max=9809  avg=42
TOP_LEVELS Latency (cycles): p50=42  p99=208  p999=334  max=8209  avg=52
BEST_ASK Latency (cycles): p50=0  p99=42  p999=83  max=2500  avg=7
BEST_BID Latency (cycles): p50=42  p99=166  p999=292  max=12291  avg=48
CANCEL_OUR Latency (cycles): p50=42  p99=125  p999=250  max=1042  avg=49
MODIFY_OUR Latency (cycles): p50=41  p99=125  p999=208  max=7458  avg=35
SEND_OUR Latency (cycles): p50=42  p99=166  p999=542  max=15291  avg=44
MODIFY Latency (cycles): p50=291  p99=625  p999=833  max=15558  avg=270
CANCEL Latency (cycles): p50=333  p99=726  p999=1000  max=54119  avg=328
ADD Latency (cycles): p50=125  p99=458  p999=667  max=40601  avg=141
"cycles_per_op": 190.08


A pretty significant improvement just with a single line change.
Never use std::list
The scattered memory makes the cache very unhappy.
