use crate::types::OrderBook;

use rustc_hash::FxHashMap;

pub struct SetOb{
    bids: FxHashMap<u64, i64>,
    asks: FxHashMap<u64, i64>
}

impl OrderBook for SetOb{
    fn new() -> Self{
        Self { bids: FxHashMap::default(), asks: FxHashMap::default() }
    }

    fn add_order(&mut self, id: u64, side: i32, price: i64, quantity: i64){
        if side == 0 {
            self.bids.insert(id, price);
        }
        else {
            self.asks.insert(id, price);
        }
    }

    fn cancel_order(&mut self, id: u64){
        self.bids.remove(&id); // does nothing if key is not in map
        self.asks.remove(&id);
    }

    fn best_bid(&self) -> i64{
        if self.bids.is_empty(){
            return 0;
        }
        let mut price = i64::MIN;
        for (_oid, p) in self.bids.iter(){
            price = std::cmp::max(price, *p);
        }
        price
    }

    fn best_ask(&self) -> i64{
        if self.asks.is_empty(){
            return 0;
        }
        let mut price = i64::MAX;
        for (_oid, p) in self.asks.iter(){
            price = std::cmp::min(price, *p);
        }
        price
    }
}

#[cfg(test)]
mod set_ut{
    use super::*;
    use crate::ut::{self};

    #[test]
    fn test_bid(){
        ut::ut_common::test_bid(SetOb::new());
    }

    #[test]
    fn test_ask(){
        ut::ut_common::test_ask(SetOb::new());
    }

    #[test]
    fn test_cancel(){
        ut::ut_common::test_cancel(SetOb::new());
    }

    #[test]
    fn test_empty(){
        ut::ut_common::test_empty(SetOb::new());
    }

    #[test]
    fn test_duplicate(){
        ut::ut_common::test_duplicate_price(SetOb::new());
    }
}
