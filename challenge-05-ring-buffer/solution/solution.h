#pragma once
// Challenge 05: Ring Buffer (SPSC)
// Edit this file and solution.cpp to implement your solution.
//
// This is a Single-Producer Single-Consumer ring buffer.
// The producer calls push() from one thread, the consumer calls pop() from another.
// You MUST ensure thread safety between the producer and consumer.

// #include "default_impl.h"
#include "atomic_counter_impl.h"