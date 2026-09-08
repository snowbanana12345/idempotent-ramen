## Summary of results ##
For implementation, see source code itself.

## STL containers ##
std::map + std::unordered_map - cycles_per_op : ~100-105 cycles
std::priority_queue + std::unordered_map - cycles_per_op : ~23.5 (deviates by ~0.3 between repeated runs)
std::unordered_set - cycles_per_op : 15400 (Oh, my ramen)

~ Comments ~
std::map - binary tree with nodes. O(log n) performance. Not cache friendly due to random access
std::priority_queue + std::undered_map - performs better because it make use of the requirement that only the top of book is needed
std::unodered_set - linear scan with random access to compute best price kills performance completely

## Boost containers ##

boost::intrusive::rbtree - ~95-96 cycles / op
boost::intrusive::treap - ~47 cycles / op 

~ Comments ~
Every implementation uses std::vector<Order> as an object pool 
boost::intrusive::rbtree - exact same algorithm as std::map, just slightly faster due to reusing pre-allocated level objects
