use crate::types::OrderBook;
use std::collections::{BTreeSet, HashMap};


pub struct OpBstOb {
    bid_prices: HashMap<u64, i64>,
    ask_prices: HashMap<u64, i64>,
    bids: BTreeSet<Level>, 
    asks: BTreeSet<Level>, 
}

#[derive(Debug, PartialEq, Eq)]
struct Level {
    price: i64,
    order_id: u64,
}

impl PartialOrd for Level {
    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
        self.price.partial_cmp(&other.price)
    }
}

impl Ord for Level {
    fn cmp(&self, other: &Self) -> std::cmp::Ordering {
        match self.price.cmp(&other.price){
            std::cmp::Ordering::Less => std::cmp::Ordering::Less,
            std::cmp::Ordering::Greater => std::cmp::Ordering::Greater,
            std::cmp::Ordering::Equal => self.order_id.cmp(&other.order_id),
        }
    }
}

impl OrderBook for OpBstOb {
    fn new() -> Self {
        Self {
            bid_prices: HashMap::new(),
            ask_prices: HashMap::new(),
            bids: BTreeSet::new(),
            asks: BTreeSet::new(),
        }
    }

    fn add_order(&mut self, id: u64, side: i32, price: i64, quantity: i64) {
        if side == 0 {
            self.bid_prices.insert(id, price);
            self.bids.insert(Level{price: price, order_id: id});
        } else {
            self.ask_prices.insert(id, price);
            self.asks.insert(Level{price: price, order_id: id});
        }
    }

    fn cancel_order(&mut self, id: u64) {
        if let Some(price) = self.bid_prices.get(&id){
            self.bids.remove(&Level {price: *price, order_id: id});
            self.bid_prices.remove(&id);
        }
        else if let Some(price) = self.ask_prices.get(&id){
            self.asks.remove(&Level {price: *price, order_id: id});
            self.ask_prices.remove(&id);
        }
    }

    fn best_bid(&self) -> i64 {
        if self.bids.is_empty(){
            return 0;
        }
        self.bids.last().unwrap().price
    }

    fn best_ask(&self) -> i64 {
        if self.asks.is_empty(){
            return 0;
        }
        self.asks.first().unwrap().price
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