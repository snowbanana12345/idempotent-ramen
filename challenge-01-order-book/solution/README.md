##
Main article is in the rust version of the challenge

Just some notes on set up for C++
- avoided declaring OrderBook in Solution.h as virtual as its not 0 cost
- Use cmake to link benchmark.cpp to various implementations
- unittesting work the same way.
- build seperate binaries with each implementation against the same unittest file.

Results are similar
Heap implementation:
cycles_per_op : 26