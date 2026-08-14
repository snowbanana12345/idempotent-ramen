// Challenge 05: Ring Buffer (SPSC) — Skeleton Implementation
// This is a correct but slow mutex-based reference. You can do MUCH better!

#include "base.h"

namespace hftu {

    class RingBuffer {
public:
    // Capacity is always a power of 2.
    explicit RingBuffer(size_t capacity)
    : buf_(capacity), capacity_(capacity) {}

    // Push a message (producer thread). Returns false if full.
    bool push(const Message& msg) {
        std::lock_guard<std::mutex> lock(mtx_);
        if (count_ == capacity_) return false;
        buf_[tail_] = msg;
        tail_ = (tail_ + 1) % capacity_;
        ++count_;
        return true;
    }

    // Pop a message into out (consumer thread). Returns false if empty.
    bool RingBuffer::pop(Message& out) {
        std::lock_guard<std::mutex> lock(mtx_);
        if (count_ == 0) return false;
        out = buf_[head_];
        head_ = (head_ + 1) % capacity_;
        --count_;
        return true;
    }

    // Number of elements currently stored.
    size_t RingBuffer::size() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return count_;
    }

private:
    std::vector<Message> buf_;
    size_t capacity_;
    size_t head_ = 0;
    size_t tail_ = 0;
    size_t count_ = 0;
    mutable std::mutex mtx_;
};

} 
