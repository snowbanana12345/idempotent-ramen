## ------ STL containers -------- ##

### std::map + std::unordered_map - ~100-105
binary tree with nodes. O(log n) performance. Not cache friendly due to random access

### std::priority_queue + std::unordered_map - ~23.5*
std::priority_queue + std::undered_map - performs better because it make use of the requirement that only the top of book is needed
NOTE : this implementation is NOT production viable. There is a case where the stale order stack up indefinately

### std::unordered_set - 9000 (Oh, my ramen)
std::unodered_set - linear scan with random access to compute best price kills performance completely

## ------  Boost containers  -------- ##
Every implementation uses std::vector as an object pool 

## boost::intrusive::rbtree - ~95-96 ## 
exact same algorithm as std::map, just slightly faster due to reusing pre-allocated level objects

## boost::intrusive::treap - ~47 
hybrid between priority_queue and binary tree. Not best because order_id should be findable in O(1) time instead of O(log n) along with the more complex processing treap needs to maintain its invariants

## boost::heap::d_ary_heap + std::unordered_set - ~19.5*
expand the number of children in the dary heap, reduce the random access from bubble up and down
checking through multiple children is cache friendly as the children as laid out sequentially in memory
NOTE : this implementation is NOT production viable. There is a case where the stale order stack up indefinately

## boost::heap::d_ary_heap with updates ~ 47
in general, we cannot efficient search for an element in the heap. But we can store its index (handle) 
in a hash map to locate it. This additional work we have to do to ensure safety does double the performance cost



