use crate::types::OrderBook;

use std::collections::HashSet;

pub struct VecOb {
    bid_st : HashSet<u64>,
    ask_st : HashSet<u64>,
    bids: Vec<Level>,  // price -> total qty (descending via Reverse)
    asks: Vec<Level>,  // price -> total qty (ascending)
}

pub struct Level{
    price: i64,
    order_id: u64
}

impl OrderBook for VecOb{
    fn new() -> Self {
        Self {
            bid_st: HashSet::new(),
            ask_st: HashSet::new(),
            bids: Vec::with_capacity(1000),
            asks: Vec::with_capacity(1000),
        }
    }

    fn add_order(&mut self, id: u64, side: i32, price: i64, quantity: i64) {
        if side == 0 {
            let mut idx: usize = 0;
            for l in self.bids.iter(){
                if l.price <= price{
                    break;
                }
                idx += 1;
            }
            self.bid_st.insert(id);
            self.bids.insert(idx, Level {price: price, order_id: id});
        } else {
            let mut idx: usize = 0;
            for l in self.asks.iter(){
                if l.price >= price{
                    break;
                }
                idx += 1;
            }
            self.ask_st.insert(id);
            self.asks.insert(idx, Level {price: price, order_id: id});
        }
    }

    fn cancel_order(&mut self, id: u64) {
        if self.bid_st.contains(&id) {
            if let Some(pos) = self.bids.iter().position(|l| l.order_id == id) {
                self.bids.remove(pos);  
                self.bid_st.remove(&id);
                return;
            }
        }
        if self.ask_st.contains(&id){
            if let Some(pos) = self.asks.iter().position(|l| l.order_id == id) {
                self.asks.remove(pos);  
                self.ask_st.remove(&id);
                return;
            }
        }
    }

    fn best_bid(&self) -> i64 {
        if let Some(best_bid) = self.bids.get(0){
            return best_bid.price;
        }
        return 0;
    }

    fn best_ask(&self) -> i64 {
        if let Some(best_ask) = self.asks.get(0){
            return best_ask.price;
        }
        return 0;
    }
}


#[cfg(test)]
mod vec_ut{
    use super::*;
    use crate::ut::{self};

    #[test]
    fn test_bid(){
        ut::ut_common::test_bid(VecOb::new());
    }

    #[test]
    fn test_ask(){
        ut::ut_common::test_ask(VecOb::new());
    }

    #[test]
    fn test_cancel(){
        ut::ut_common::test_cancel(VecOb::new());
    }

    #[test]
    fn test_empty(){
        ut::ut_common::test_empty(VecOb::new());
    }
    
    #[test]
    fn test_duplciate(){
        ut::ut_common::test_duplicate_price(VecOb::new());
    }
}