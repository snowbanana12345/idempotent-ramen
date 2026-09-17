## --- workload characteristics ---
100k keys total
must return nullptr when key not exist but most keys do exist
max 16 length keys with uniform distribution across key sizes


## default implementation ##

cycles_per_op : 34.8 (local)
cycles_per_op : 387 (submission)


## store short strings ##

cycles_per_op : 18.4 (local)
Stores two character strings in array that does not require string hashing
Storing three character strings does not making it faster.

## FNV hash ##

cycles_per_op : 27.22 (local)
cycles_per_op : 280 (submission)

use FNV hash as hash function

## Boost::hash_range ## 

cycles_per_op: 28.0 (local)
boost::hash_range does roughly the same thing just slightly less efficient.

## Putting it togther ## 

cycles_per_op: 17 (local)
cycles_per_op : 190 (submission)

short two character strings goes into array
longer strings goes into unordered_map

## fnv hash parallelized ##
compute hash of 0-4 char, 0-8 char etc using instruction level parallelism
saves ~10 cycles/op on submission server

## fnv FNV hash minimzlied ##

cycles_per_op : 15 (local)
cycles_per_op : 154 (submission)
our hash function only needs to prevent enough hash collisions for
the workload with is very sparse. The minimal needed is to hash 5 out of the 16 characters plus
the key_len

## custom hash map ##
cycles_per_op: 20.5 (local)
our own implementation of a hashmap.
compute hash + L3 access for whole string + string compare
use normal std::unordered_map for collision handling, small enough to fit into L1

