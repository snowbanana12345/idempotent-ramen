use crate::types::OrderBook;
use std::collections::{BTreeMap, HashMap};


/*

Default implementation provided. 

add_order(id, side, price, quantity): log(n)
cancel_order(id): log(n)
best_bid(): log(n)
best_ask(): log(n)

*/

pub struct BTreeOb {
    orders: HashMap<u64, Order>,
    bids: BTreeMap<i64, i64>,  // price -> total qty (descending via Reverse)
    asks: BTreeMap<i64, i64>,  // price -> total qty (ascending)
}

struct Order {
    side: i32,
    price: i64,
    quantity: i64,
}

impl OrderBook for BTreeOb {
    fn new() -> Self {
        Self {
            orders: HashMap::new(),
            bids: BTreeMap::new(),
            asks: BTreeMap::new(),
        }
    }

    fn add_order(&mut self, id: u64, side: i32, price: i64, quantity: i64) {
        self.orders.insert(id, Order { side, price, quantity });
        if side == 0 {
            *self.bids.entry(price).or_insert(0) += quantity;
        } else {
            *self.asks.entry(price).or_insert(0) += quantity;
        }
    }

    fn cancel_order(&mut self, id: u64) {
        let Some(order) = self.orders.remove(&id) else {
            return;
        };
        if order.side == 0 {
            if let Some(qty) = self.bids.get_mut(&order.price) {
                *qty -= order.quantity;
                if *qty <= 0 {
                    self.bids.remove(&order.price);
                }
            }
        } else {
            if let Some(qty) = self.asks.get_mut(&order.price) {
                *qty -= order.quantity;
                if *qty <= 0 {
                    self.asks.remove(&order.price);
                }
            }
        }
    }

    fn best_bid(&self) -> i64 {
        // BTreeMap is ascending, so last entry is highest
        self.bids.keys().next_back().copied().unwrap_or(0)
    }

    fn best_ask(&self) -> i64 {
        // BTreeMap is ascending, so first entry is lowest
        self.asks.keys().next().copied().unwrap_or(0)
    }
}


#[cfg(test)]
mod btree_ut{
    use super::*;
    use crate::ut::{self};

    #[test]
    fn test_bid(){
        ut::ut_common::test_bid(BTreeOb::new());
    }

    #[test]
    fn test_ask(){
        ut::ut_common::test_ask(BTreeOb::new());
    }

    #[test]
    fn test_cancel(){
        ut::ut_common::test_cancel(BTreeOb::new());
    }

    #[test]
    fn test_empty(){
        ut::ut_common::test_empty(BTreeOb::new());
    }
}