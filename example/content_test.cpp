#include <iostream>
#include <string>
#include <assert.h>
#include <chrono>
#include <vector>
#include <unordered_map>

#include <Slab.h>
#include <Items.h>
#include <MemCache.h>
#include <HashTable.h>
#include <Page.h>

using namespace std;
using namespace moxie;

#define M (1014 * 1024)

void insert_find(size_t item, const std::string& value) {
    int chunk_size = value.size() + 150;
    MemCache *cache = new (std::nothrow) MemCache(chunk_size, 1.25, 40, DEFAULT_POWER_BLOCK, 64 * DEFAULT_POWER_BLOCK, 1);
    if (!cache) {
        std::cout << "New Memcache failed!" << std::endl;
        return;
    }

    std::string key_prefix;
    key_prefix.resize(30, 'a');

    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    HashTable *hashtable_two = new HashTable(20, 1);
    for (size_t i = 0; i < item; ++i) {
        std::string key = key_prefix + std::to_string(i);
        std::string new_value = value + key;
        Item *it = cache->create_item(key.c_str(), key.size(), new_value.c_str(), new_value.size());
        if (!it) {
            std::cout << "create item[" << i << "] failed!" << std::endl;
            return;
        }
        
        hashtable_two->item_insert(it);
        assert(hashtable_two->item_find(key.c_str(), key.size()));
    }
    std::chrono::steady_clock::time_point middle = std::chrono::steady_clock::now();
    for (size_t i = 0; i < item; ++i) {
        std::string key = key_prefix + std::to_string(i);
        assert(hashtable_two->item_find(key.c_str(), key.size()));
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "adjust_times:" << cache->adjust_times() << " insert_time:" << std::chrono::duration_cast<std::chrono::microseconds>(middle - start).count() 
                << " find_time:" << std::chrono::duration_cast<std::chrono::microseconds>(end - middle).count() << std::endl;
    delete hashtable_two;
    delete cache;
}

int main() {
    std::string value = "v";
    std::vector<size_t> items = {10000000};
    std::vector<size_t> valuelen = {128, 256, 478, 512, 640, 768, 896, 1024};
    for (size_t i = 0; i < valuelen.size(); ++i) {
        std::cout << "valuelen" << valuelen[i] << std::endl;
        value.resize(valuelen[i], 'v');
        for (size_t j = 0; j < items.size(); ++j) {
            insert_find(items[j], value);
        }
        std::cout << std::endl;
    }
    return 0;
}
