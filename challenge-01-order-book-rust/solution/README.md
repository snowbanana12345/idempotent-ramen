## Introduction
Explainations of attempted solutions for challenge-1
All implementations satisfy the same set of unittests
Big O matters here because the problem constraints state 1 million active orders at a time
all 1 million orders will not fit into cache

## Work load
Mixed, 60% add, 20% cancel, 10% best_bid(), 10% best_ask()
up to 1 million active orders


## BTree
default implementation provided
- **`add_order(id, side, price, quantity)`** — log(n)
- **`cancel_order(id)`** — log(n)
- **`best_bid()`** — log(n)
- **`best_ask()`** — log(n)

cycles_per_op ~ 70-75
very cache unfriendly as nodes allocated on heap are scattered.

Optimizations
using smaller data structs. u32 for counts, do not need to aggregate quantity
No improvement to cycle count

Use better hashing FxHashMap.
faster but less crypgraphic security (not relevant here)
cycles_per_op ~ 65


## Vector
on add, linear scan to find the slot to insert order to maintain sorted
on cancel, linear scan to find the slot with 
- **`add_order(id, side, price, quantity)`** — O(n)
- **`cancel_order(id)`** — O(n)
- **`best_bid()`** — O(1)
- **`best_ask()`** — O(1)

very poor as vector needs to push back all elements on add
push forward all elements on cancel
best_bid() and best_ask() operations involves only single array access but they are called infrequently
cycle_per_op > 1000
cache friendly as memory are together

Optimizations
There is no need to preserve the relative order of levels within the array
on add, push level to back of array. 
if new order is better price, swap the current best price to back of array
and replace the first slot in array with new best price

on cancel
linear scan for the Id, perform swap_remove to swap the back element with the deleted slot
if best_price is cancelled, linear scan the array for the next best price

The linear scan is cache friendly. But the swap is not given that the entire vector does not fit into cache.
best_bid() - single array access
best_ask() - single array access
