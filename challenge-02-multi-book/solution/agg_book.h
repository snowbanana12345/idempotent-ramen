
#include <map>
#include "../venue.h"

namespace hftu{
    template <bool ascending>
    class AggBook{
        public:
            void add(int64_t price, int64_t quantity){
                auto it = book.find(price);
                if (it == book.end()){
                    book[price] = {1, quantity};
                }
                else {
                    book[price].count++;
                    book[price].quantity += quantity;
                }
            }

            void modify(int64_t price, int64_t change_quantity){
                book[price].quantity += change_quantity;
            }

            void remove(int64_t price, int64_t quantity){
                book[price].count--;
                book[price].quantity -= quantity;
                if (book[price].count == 0){
                    book.erase(price);
                }
            }

            TopLevel best() const{
                if (book.empty()) return {0, 0, 0};
                if (ascending){
                    auto it = book.begin();
                    return {it->first, it->second.quantity, it->second.count};
                }
                else {
                    auto it = book.rbegin();
                    return {it->first, it->second.quantity, it->second.count};
                }
            }

            int get_top_levels(int n, TopLevel* out) const {
                if (ascending){
                    auto it = book.begin();
                    int i = 0;
                    while(i < n && it != book.end()){
                        out[i].price = it->first;
                        out[i].qty = it->second.quantity;
                        out[i].count = it->second.count;
                        it++; i++;
                    }
                    return i;
                }
                else {
                    auto it = book.rbegin();
                    int i = 0;
                    while(i < n && it != book.rend()){
                        out[i].price = it->first;
                        out[i].qty = it->second.quantity;
                        out[i].count = it->second.count;
                        it++; i++;
                    }
                    return i;
                }
            }

            int64_t volume_near_best(int64_t depth) const {
                if (depth == 0 || book.empty()) return 0;
                if (ascending){
                    auto it = book.begin();
                    int64_t best_price = it->first;
                    int64_t volume = 0;
                    for (; it != book.end() && it->first <= best_price + depth - 1; it++){
                        volume += it->second.quantity;
                    }
                    return volume;
                }
                else{
                    auto it = book.rbegin();
                    int64_t best_price = it->first;
                    int64_t volume = 0;
                    for (; it != book.rend() && it->first >= best_price - depth + 1; it++){
                        volume += it->second.quantity;
                    }
                    return volume;
                }
            }

        private:
            struct Level{
                int32_t count;
                int64_t quantity;
            };
            std::map<int64_t, Level> book;
    };
}
