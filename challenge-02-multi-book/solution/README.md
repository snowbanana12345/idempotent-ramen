## Introduction ##
The criteria is to optimize for p99 across all operations.
i.e. optimize for the worst case.

big O is important here because linear scans over 500_000 elements is still very slow even if its cache friendly
The 500k elements are distributed across 200 symbols but 50 of them have 90% of the symbols.
So we are still dealing with a large dataset in the worst that we are required to optimize for.
assume 100k elements for the hottest book.

As the README suggest, vector expansions, expensive binary tree rebalancing, linear scans are
rare paths that will kill the p99 latency. It means we strive for at log(n) operations on everything.

Generally, what we can do is to maintain seperate data structures to service each query
This will worsen the average latency as we will need to update those seperate data structures
But it means we can control the worst case latencies for each one.


## Methods ##
*void send_order(uint64_t our_id, uint16_t symbol, int side, int64_t price, int64_t qty)*
*void modify_our_order(uint64_t our_id, int64_t new_price, int64_t new_qty)*
*void cancel_our_order(uint64_t our_id)*

There these three methods only works with the mapping our_id -> exchange_id;
A hashmap will allow O(1) operations on all of them.

*void add_order(uint64_t exchange_id, uint16_t symbol, int side, int64_t price, int64_t qty)*
*void modify_order(uint64_t exchange_id, int64_t new_qty)*
*void cancel_order(uint64_t exchange_id)*

The case that exchange_ids that are not ours requires an O(1) hashmap check.
It eliminates methods where we skip the check and go straight to search our data structure.

*TopLevel best_bid(uint16_t symbol) const*
*TopLevel best_ask(uint16_t symbol) const*
*int get_top_levels(uint16_t symbol, int side, int n, TopLevel* out) const*

- implement this as a tree, a priority queue will do well for just the best_bid/ask
But the get_top_levels requires a traversal in order.
- get_top_levels may trigger a full linear scan, but we can't do anything about it as
the functional requirements can demand a full linear scan to write the entire book to TopLevel* out

*int64_t volume_near_best(uint16_t symbol, int side, int64_t depth) const*
*QueuePosition get_queue_position(uint64_t our_id) const*

Augmented binary tree
- sort by price, store the total quantity in each node representing 


## Default implementation

avg ~= 300 cycles / op
p99 ~= 1500 cycles /op

