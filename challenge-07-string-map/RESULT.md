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