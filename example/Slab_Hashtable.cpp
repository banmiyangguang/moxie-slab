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
    MemCache *cache = new (std::nothrow) MemCache(chunk_size, 
                                                    1.25,
                                                    40,
                                                    chunk_size * 1024,
                                                    512 * DEFAULT_POWER_BLOCK * 6,
                                                    1);
    if (!cache) {
        std::cout << "New Memcache failed!" << std::endl;
        return;
    }

    HashTable *hashtable_two = new HashTable(20, 1);
    if (!hashtable_two) {
        std::cout << "New HashTable failed!" << std::endl;
        return;
    }
    std::string key_prefix;
    key_prefix.resize(30, 'a');

    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    for (size_t i = 0; i < item; ++i) {
        std::string key = key_prefix + std::to_string(i);
        std::string new_value = value + key;
        Item *it = cache->create_item(key.c_str(), key.size(), new_value.c_str(), new_value.size());
        if (!it) {
            std::cout << "create item failed!" << std::endl;
            return;
        }
        
        hashtable_two->item_insert(it);
    }
    std::chrono::steady_clock::time_point middle = std::chrono::steady_clock::now();
    for (size_t i = 0; i < item; ++i) {
        std::string key = key_prefix + std::to_string(i);
        if (!hashtable_two->item_find(key.c_str(), key.size())) {
            std::cout << "Find Item error!" << std::endl;
        }
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    uint64_t insert_time = std::chrono::duration_cast<std::chrono::microseconds>(middle - start).count();
    uint64_t find_time = std::chrono::duration_cast<std::chrono::microseconds>(end - middle).count();
    int64_t insert_per_sec = 1000000 * item / insert_time;
    int64_t find_per_sec = 1000000 * item / find_time;
    std::cout   << "datalen:" << value.size() << " times:" << item
                << " insert_time:" << insert_time << " insert_per_sec:" << insert_per_sec
                << " find_time:" << find_time << " find_per_sec:" << find_per_sec
                << std::endl;
    delete hashtable_two;
    delete cache;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        std::cout << "Usage:exec datalen reqs" << std::endl;
        return -1;
    }
    int reqs = std::atoi(argv[2]);
    int datalen = std::atoi(argv[1]);
    if (reqs < 0 || datalen < 0) {
        std::cout << "error:reqs < 0 || datalen < 0" << std::endl;
        return -1;
    }
    std::string value;
    value.resize(datalen, 'v');
    insert_find(reqs, value);
    return 0;
}
