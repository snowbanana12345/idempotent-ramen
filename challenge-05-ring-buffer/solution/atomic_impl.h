#include "base.h"

namespace hftu {

class RingBuffer {
    public:

    explicit RingBuffer(size_t capacity)
    : buf_(capacity), capacity_(capacity), count_(0) {
        buf_.reserve(capacity);
    }

    bool push(const Message& msg) {
        if (count_.load(std::memory_order_acquire) == capacity_) return false;
        buf_[producer_ptr_] = msg;
        producer_ptr_ = (producer_ptr_ + 1) % capacity_;
        count_.fetch_add(1, std::memory_order_release);
        return true;
    }

    bool pop(Message& out) {
        if (count_.load(std::memory_order_acquire) == 0) return false;
        out = buf_[consumer_ptr_];
        consumer_ptr_ = (consumer_ptr_+ 1) % capacity_;
        count_.fetch_add(-1, std::memory_order_release);
        return true;
    }

    size_t size() const {
        return count_.load(std::memory_order_relaxed);
    }

    private:
        std::vector<Message> buf_;
        size_t capacity_; // constant always thread safe
        size_t producer_ptr_ = 0; // accessed only by producer thread
        size_t consumer_ptr_ = 0; // accessed only by consumer thread
        std::atomic<size_t> count_;
};

} 
