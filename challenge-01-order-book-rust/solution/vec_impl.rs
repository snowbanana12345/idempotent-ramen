use crate::types::OrderBook;

use rustc_hash::FxHashSet;

pub struct VecOb {
    bid_st : FxHashSet<u64>,
    ask_st : FxHashSet<u64>,
    bids: Vec<Level>,  // price -> total qty (descending via Reverse)
    asks: Vec<Level>,  // price -> total qty (ascending)
}

#[derive(Clone)]
pub struct Level{
    price: i64,
    order_id: u64
}

impl OrderBook for VecOb{
    fn new() -> Self {
        Self {
            bid_st: FxHashSet::default(),
            ask_st: FxHashSet::default(),
            bids: Vec::with_capacity(1000),
            asks: Vec::with_capacity(1000),
        }
    }

    fn add_order(&mut self, id: u64, side: i32, price: i64, quantity: i64) {
        if side == 0 {
            if !self.bids.is_empty() && price > self.bids[0].price{
                self.bids.push(self.bids[0].clone());
                self.bids[0].price = price;
                self.bids[0].order_id = id;
            }
            else {
                self.bids.push(Level {price: price, order_id: id});
            }
            self.bid_st.insert(id);
        } else {
            if !self.asks.is_empty() && price < self.asks[0].price{
                self.asks.push(self.asks[0].clone());
                self.asks[0].price = price;
                self.asks[0].order_id = id;
            }
            else {
                self.asks.push(Level {price: price, order_id: id});
            }
            self.ask_st.insert(id);
        }
    }

    fn cancel_order(&mut self, id: u64) {
        if self.bid_st.contains(&id) {
            if self.bids[0].order_id == id{ // yikes, we have to search for the max again.
                self.bids.swap_remove(0);
                if self.bids.is_empty(){
                    return;
                }
                let mut idx : usize = 0;
                let mut mx_price: i64 = i64::MIN;
                for i in 0..self.bids.len(){
                    if self.bids[i].price > mx_price{
                        idx = i;
                        mx_price = self.bids[i].price;
                    }
                }
                self.bids.swap(0, idx);
            }
            else {
                for i in 0..self.bids.len(){
                    if self.bids[i].order_id == id{
                        self.bids.swap_remove(i);
                        break;
                    }
                }
            }
        }
        if self.ask_st.contains(&id){
            if self.asks[0].order_id == id{ // yikes, we have to search for the max again.
                self.asks.swap_remove(0);
                if self.asks.is_empty(){
                    return;
                }
                let mut idx : usize = 0;
                let mut mx_price: i64 = i64::MAX;
                for i in 0..self.asks.len(){
                    if self.asks[i].price < mx_price{
                        idx = i;
                        mx_price = self.asks[i].price;
                    }
                }
                self.asks.swap(0, idx);
            }
            else {
                for i in 0..self.asks.len(){
                    if self.asks[i].order_id == id{
                        self.asks.swap_remove(i);
                        break;
                    }
                }
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