use crate::types::OrderBook;
use std::collections::{BinaryHeap, HashSet};

pub struct HeapOb{
    id_st: HashSet<u64>,
    bids: BinaryHeap<Level>,
    asks: BinaryHeap<Level>
}

#[derive(PartialEq, Eq)]
struct Level{
    price: i64,
    order_id: u64
}

impl PartialOrd for Level {
    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
        self.price.partial_cmp(&other.price)
    }
}

impl Ord for Level {
    fn cmp(&self, other: &Self) -> std::cmp::Ordering {
        self.price.cmp(&other.price)
    }
}

impl OrderBook for HeapOb{
    fn new() -> Self{
        Self{
            id_st: HashSet::new(),
            bids: BinaryHeap::new(),
            asks: BinaryHeap::new(),
        }
    }

    fn add_order(&mut self, id: u64, side: i32, price: i64, quantity: i64){
        self.id_st.insert(id);
        if side == 0{
            self.bids.push(Level {price: price, order_id: id});
        }
        else {
            self.asks.push(Level {price: - price, order_id: id});
        }
    }

    fn cancel_order(&mut self, id: u64){
        self.id_st.remove(&id);

        while !self.bids.is_empty(){ // ensure that the best bid is still valid
            if let Some(level) = self.bids.peek(){
                if self.id_st.contains(&level.order_id){
                    break;
                }
            }
            self.bids.pop();
        }

        while !self.asks.is_empty(){ // ensure that the best ask is still valid
            if let Some(level) = self.asks.peek(){
                if self.id_st.contains(&level.order_id){
                    break;
                }
            }
            self.asks.pop();
        }
    }

    fn best_bid(&self) -> i64{
        match self.bids.peek(){
            Some(level) => level.price,
            None => 0
        }
    }

    fn best_ask(&self) -> i64{
        match self.asks.peek(){
            Some(level) => - level.price,
            None => 0
        }
    }
}


#[cfg(test)]
mod heap_ut{
    use super::*;
    use crate::ut::{self};

    #[test]
    fn test_bid(){
        ut::ut_common::test_bid(HeapOb::new());
    }

    #[test]
    fn test_ask(){
        ut::ut_common::test_ask(HeapOb::new());
    }

    #[test]
    fn test_cancel(){
        ut::ut_common::test_cancel(HeapOb::new());
    }

    #[test]
    fn test_empty(){
        ut::ut_common::test_empty(HeapOb::new());
    }

    #[test]
    fn test_duplicate(){
        ut::ut_common::test_duplicate_price(HeapOb::new());
    }
}