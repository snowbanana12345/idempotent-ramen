#include <cstdint>
#include <map>

namespace lemon{
    class AvlTree{
        public:
            void insert(uint64_t key, uint64_t value);
            void remove(uint64_t key);
            void update(uint64_t key, uint64_t new_value);
            uint64_t get(uint64_t key);
            uint64_t sum_floor(uint64_t key);
            uint64_t sum_floor_inclusive(uint64_t key);

        private:


            struct Node{
                uint64_t key;
                uint64_t value;
                uint64_t sum;
                uint32_t height;
            };

            Node* root;
    };
}