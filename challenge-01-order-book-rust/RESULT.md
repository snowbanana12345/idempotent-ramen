## std::collections BTree ~ 73 ##
Default implementation Binary tree. log(n) across the board. Nodal structure makes it cache unfriendly.
Replacing hash in stl with FxHash saves ~3-5 cycles

## std::Vector > 1000 ##
Very inefficient due to the need to move whole segments of an array
Vectorization and instruction level parallism is still not going to save us.

## std::collections::Heap + FxHash ~ 11 ##
Track best price by heap, invalidate 

## rust_hash::FxHashMap > 10000 (on my ramen) ##
linear scan through a hash map is very slow

