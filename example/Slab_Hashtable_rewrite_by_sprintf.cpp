#include <iostream>
#include <string>
#include <string.h>
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

#define DEFAULT_KEY_SIZE 30
#define DEFAULT_KEY_PAD 256
#define DEFAULT_VALUE_PAD DEFAULT_KEY_PAD

static inline uint64_t time_now(void) {
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return ((uint64_t) time.tv_sec) * 1000000000 + time.tv_nsec;
}

void insert_find(size_t items, size_t value_size) {
    int chunk_size = value_size + 150;
/*
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
*/
    char *key_prefix = (char *)malloc(DEFAULT_KEY_SIZE),
         *value_prefix = (char *)malloc(value_size);
    char *key = (char *)malloc(DEFAULT_KEY_PAD),
         *value = (char *)malloc(value_size + DEFAULT_VALUE_PAD);
    if (!key_prefix || !value_prefix || !key || !value) {
        std::cout << "Malloc key or value space failed!\n";
        free(key_prefix);
        free(value_prefix);
        free(key);
        free(value);
        return;
    }
    memset(key_prefix, 'K', DEFAULT_KEY_SIZE - 1);
    memset(value_prefix, 'V', value_size - 1);
    key_prefix[DEFAULT_KEY_SIZE - 1] = 0;
    value_prefix[value_size - 1] = 0;
    
    uint64_t start_set = time_now() / 1000;
    size_t i = 0;
    for (; i < items; ++i) {
        int key_size = sprintf(key, "%s%d", key_prefix, i);
        int value_size = sprintf(value, "%s%s", key, value_prefix);
/*
        Item *it = cache->create_item(key, key_size, 0, value, value_size);
        if (!it) {
            std::cout << "create item failed!" << std::endl;
            return;
        }

        hashtable_two->item_insert(it);
*/
    }
    uint64_t end_set = time_now() / 1000;
    for (size_t j = 0; j < i; ++j) {
        int key_size = sprintf(key, "%s%d", key_prefix, j);
 /*
        if (!hashtable_two->item_find(key, key_size)) {
            std::cout << "Find item failed!" << std::endl;
        }
*/
    }
    uint64_t end_get = time_now() / 1000;
    int64_t insert_time = end_set - start_set;
    int64_t find_time = end_get - end_set;
    int64_t insert_per_sec = i * 1000000 / insert_time;
    int64_t find_per_sec = i * 1000000/ find_time;

    printf("datalen:%u times:%u insert_time:%lld insert_per_sec:%lld find_time:%lld find_per_sec:%lld\n",
            value_size, items, insert_time, insert_per_sec, find_time, find_per_sec);
    
 //   delete hashtable_two;
 //   delete cache;
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
    insert_find(reqs, datalen);
    return 0;
}
