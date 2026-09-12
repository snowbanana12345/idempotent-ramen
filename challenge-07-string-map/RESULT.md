## default implementation ##

cycles_per_op : 34.8 (local)


## store short strings ##

cycles_per_op : 31.72 (local)
Stores two character strings in array that does not require string hashing
Storing three character strings does not making it faster.


## Custom hash ##

cycles_per_op : 27.22 (local)
instead of creating a string and then calling hash function on string,
directly hash the raw arguments