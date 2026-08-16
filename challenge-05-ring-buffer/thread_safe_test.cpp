#include "solution/solution.h"
#include "thread"

static constexpr uint32_t TOTAL_OPS = 1'000'000;

int main(){
    hftu::RingBuffer rb(2048);
    std::vector<hftu::Message> published;
    published.reserve(TOTAL_OPS);
    std::vector<hftu::Message> consumed;
    consumed.reserve(TOTAL_OPS);

    std::atomic_bool consumer_ready(false);

    std::thread consumer([&]() {
        hftu::Message msg;
        consumer_ready.store(true, std::memory_order_release);
        while(consumed.size() < TOTAL_OPS) {
            if (rb.pop(msg)){
                consumed.push_back(msg);
            }
        }
    });

     while (!consumer_ready.load(std::memory_order_acquire)) {}

    std::thread producer([&]() {
        hftu::Message msg;
        for (size_t i = 0; i < TOTAL_OPS; ++i) {
            msg.timestamp = static_cast<int64_t>(i);
            msg.sequence = i;
            msg.symbol_id = static_cast<uint32_t>(i & 0xFFF);
            msg.side = static_cast<uint16_t>(i & 1);
            msg.price = static_cast<int64_t>(i * 100 + 1);
            msg.quantity = static_cast<int64_t>((i & 0xFF) + 1);
            msg.order_id = static_cast<int64_t>(i);
            while (!rb.push(msg)) {
                
            }
            published.push_back(msg);
        }
    }); 

    producer.join();
    consumer.join();

    if (published.size() != consumed.size()){
        std::cout << "publish != consumed count : " << published.size() << " " << consumed.size() << std::endl;
        return 0;
    }

    for (int i = 0; i < TOTAL_OPS; i++){
        if (published[i] != consumed[i]){
            std::cout << "message mismatch at index : " << i << std::endl;
            return 0;
        }
    }

    std::cout << "successfully passed thread safety test" << std::endl;

    return 0;
}