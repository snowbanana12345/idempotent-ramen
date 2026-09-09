use crate::types::OrderBook;
use std::collections::{BTreeMap};
use rustc_hash::FxHashMap;

pub struct OpBstOb {
    orders: FxHashMap<u64, OrderLite>,
    bids: BTreeMap<i64, u32>, 
    asks: BTreeMap<i64, u32>, 
}

struct OrderLite{
    price: i64,
    is_buy: bool,
}

impl OrderBook for OpBstOb {
    fn new() -> Self {
        Self {
            orders: FxHashMap::default(),
            bids: BTreeMap::new(),
            asks: BTreeMap::new(),
        }
    }

    fn add_order(&mut self, id: u64, side: i32, price: i64, quantity: i64) {
        self.orders.insert(id, OrderLite {price: price, is_buy: side == 0});
        if side == 0 {
            self.bids.entry(price).and_modify(|e| *e += 1).or_insert(1);
        } else {
            self.asks.entry(price).and_modify(|e| *e += 1).or_insert(1);
        }
    }

    fn cancel_order(&mut self, id: u64) {
        let Some(order) = self.orders.remove(&id) else {
            return;
        };
        if order.is_buy{
            if let Some(1) = self.bids.get(&order.price) {
                self.bids.remove(&order.price);
            }
            else {
                self.bids.entry(order.price).and_modify(|e| *e -= 1);
            }
        }
        else {
            if let Some(1) = self.asks.get(&order.price) {
                self.asks.remove(&order.price);
            }
            else {
                self.asks.entry(order.price).and_modify(|e| *e -= 1);
            }
        }
    }

    fn best_bid(&self) -> i64 {
        if let Some(price) = self.bids.keys().next_back() {
            return *price;
        }
        0
    }

    fn best_ask(&self) -> i64 {
        if let Some(price) = self.asks.keys().next(){
            return *price;
        }
        0
    }
}


#[cfg(test)]
mod op_btree_ut{
    use super::*;
    use crate::ut::{self};

    #[test]
    fn test_bid(){
        ut::ut_common::test_bid(OpBstOb::new());
    }

    #[test]
    fn test_ask(){
        ut::ut_common::test_ask(OpBstOb::new());
    }

    #[test]
    fn test_cancel(){
        ut::ut_common::test_cancel(OpBstOb::new());
    }

    #[test]
    fn test_empty(){
        ut::ut_common::test_empty(OpBstOb::new());
    }

    #[test]
    fn test_duplicate(){
        ut::ut_common::test_duplicate_price(OpBstOb::new());
    }
}