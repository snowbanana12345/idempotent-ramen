#[cfg(test)]
pub mod ut_common{
    use crate::types::OrderBook;

    pub fn test_bid<T>(mut ob : T ) where T: OrderBook
    {
        ob.add_order(1, 0, 100 ,5);
        assert_eq!(ob.best_bid(), 100);
        ob.add_order(2, 0, 99 ,5);
        ob.add_order(3, 0, 101 ,5);
        assert_eq!(ob.best_bid(), 101);
    }

    pub fn test_ask<T>(mut ob : T ) where T: OrderBook
    {
        ob.add_order(1, 1, 172 ,5);
        assert_eq!(ob.best_ask(), 172);
        ob.add_order(2, 1, 170 ,5);
        ob.add_order(3, 1, 175 ,5);
        ob.add_order(4, 1, 174 ,5);
        assert_eq!(ob.best_ask(), 170);
    }

    pub fn test_cancel<T>(mut ob: T) where T: OrderBook
    {
        ob.add_order(1, 1, 172 ,5);
        ob.add_order(2, 1, 173 ,5);
        assert_eq!(ob.best_ask(), 172);
        ob.cancel_order(1);
        assert_eq!(ob.best_ask(), 173);
    }

    pub fn test_empty<T>(mut ob: T) where T: OrderBook
    {
        assert_eq!(ob.best_ask(), 0);
        assert_eq!(ob.best_bid(), 0);
    }
}