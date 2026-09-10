## Introduction

Easier version of the event scheduler that has to support cancel and reschedule.
The data distribution is the same

## std::map
cycles_per_op: 334.00
binary tree has bad cache priorities

## std::priority_queue
cycles_per_op: 332.00
jumping arrays using a heap is also bad caching


## slotted heap 
see files : slot_heap_impl.h, slot_heap.h
cycles_per_op : 250.00

uses priority_queue as a base but break up into near,mid,far
only near is only accessed during advance, storing all in the same priority_queue
forces us to manage far out events to maintain the data structure invariants which costs performance.

## NanoBucket
see files : nano_bucket_impl.h nano_bucket.h slot_heap.h
cycles_per_op : 168

goal is go optimize P99. P99 is caused by large advances triggering many events.
Fastest way to do batch process is if data is already stored in sorted array.
reuse the slot_heap implementation for cold_bucket.

