#include "slot_heap.h"
#include <gmock/gmock.h>
#include <vector>


using namespace hftu;


struct UTHeap : public SlottedHeaps<int, UTHeap, 2, 10> {
    struct Record{
        int e;
        int64_t t;
    };

    std::vector<Record> fires;
    std::vector<Record> moves;

    void fire(int e, int64_t t) {
         fires.push_back({e, t});
    }

    void move(int e, int64_t t) {
         moves.push_back({e, t});
    }
};

