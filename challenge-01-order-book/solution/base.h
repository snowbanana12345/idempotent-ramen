#pragma once
#include <cstdint>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>

namespace hftu{
    struct Order {
        int64_t price;
        uint64_t id;

        Order(int64_t p, uint64_t i) : price(p), id(i) {}
    };

    struct Ascending {
        bool operator()(const Order &a, const Order &b) { return a.price < b.price; };
    };
}