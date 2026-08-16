## Note on benchmarking ##

benchmarks on local laptop is very unreliable. 
Huge fluctuations between repeated benchmark runs.

## default implementation ##
acquire mutax locks. context switch. slow. bad.

## atomic on counter ##

see file : atomic_counter_impl.h

how it works : the count_ functions as a gate.

When buffer is empty, producer writes the new message to buffer and increments count_
consumer sees that count_ > 0 and polls a message

When buffer is full, producer polls a message and decrements count_
producer sees that count_ is less than capacity, producer can push another message

std::memory_order_release - everything that happened before will be visible before I increment count_
std::memory_order_acquire - I want to see everything that happened before I load count_

When buffer is empty, producer writes message and increments count_ with memory_order_release to make the new message
visible to consumer

When buffer is full, consumer reads the message with memory order and decrements count_ with memory_order_release. The memory order prevents
the producer from overwriting the data until the consumer had read it.


## memory order relaxed ##
Can we use a weaker memory order thereby improving efficiency.

bool push(const Message& msg) {
    if (count_.load(std::memory_order_acquire) == capacity_) return false; <--- (1)
    buf_[producer_ptr_] = msg;
    producer_ptr_ = (producer_ptr_ + 1) % capacity_;
    count_.fetch_add(1, std::memory_order_release);  <--- (2)
    return true;
}

bool pop(Message& out) {
    if (count_.load(std::memory_order_acquire) == 0) return false;  <--- (3)
    out = buf_[consumer_ptr_];
    consumer_ptr_ = (consumer_ptr_+ 1) % capacity_;
    count_.fetch_add(-1, std::memory_order_release);  <--- (4)
    return true;
}

(1) This can be memory order relaxed. Consumer does not write anything to it. 
Perfectly safe to write it memory_order_acquire
(4) This can also be memory_order_relaxed. The data in the consumer thread's register. 
The fetch_add will open the gate for the producer to keep writing, but immediately to its own register.
Simply, consumer thread is allowed to update the atomic counter first, and then read the buffer.


