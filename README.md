# Introduction

Solutions to micro optimiation problems posed by HFTU in both C++ and Rust. This is not an answer key. 
This is a research journal. It contains bad solutions and very very bad solutions that might set your CPU on fire.
Always measure, a brillant idea is probablely wrong.

# Layout

For each challenge
- RESULT.md contains a summary of the various implementations and their final result along with a brief discussion
- solution folder contains actual implementations
- explanations how the implementations work is in the implementation


# General notes on optimization

- sequential access is very fast
- removing a branch saves << 5 cycles avereage at the very most
- std::list is b**ls
- std::vector with pointer performs better than boost::object_pool but has object lifetime hazard
- FxHash in rust is more efficient but lower security which is not needed here

# Issue of 1 million elements

- does not fit into the L1 cache
- does not fully fit into the l2 cache 
- fits into L3 cache
- A random hashmap access likely involves pulling from L3 cache
- big O notation matters at this size
- binary tree outperforms linear scan despite the latter being cache friendly