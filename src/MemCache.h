#ifndef MOXIE_MEMCACHE_H
#define MOXIE_MEMCACHE_H
#include "Slab.h"
#include "Items.h"

/* powers-of-N allocation structures */
namespace moxie {

const uint32_t DEFAULT_POWER_SMALLEST  = 1;
const uint32_t DEFAULT_POWER_LARGEST = 200;
const uint64_t DEFAULT_POWER_BLOCK = 1048576; /* 1M */

class MemCache {
public:
    MemCache(size_t base_chunk_size, 
        double factor, 
        size_t power_largest, 
        uint64_t power_block, 
        uint64_t mem_limit, 
        int pre_alloc);
    ~MemCache();
    void *mem_cache_alloc(size_t size, size_t slab_id);
    bool mem_cache_free(void *ptr, size_t size);
    bool mem_cache_free(int slab_id, void *ptr);
    size_t mem_cache_clsid(size_t size);
    moxie::Item *create_item(const char *key, size_t keylen, const char* data, size_t nbytes);
    void recycle_item(Item *it);
    bool addjust_pages_to_slab(size_t slab_id);

    size_t adjust_times() const;
private:
    size_t adjust_times_;
    double factor;
    size_t base_chunk_size;
    size_t power_block;
    size_t power_smallest;
    size_t power_largest;
    uint64_t mem_limit;
    uint64_t mem_malloced;
    Slab **slabclass; /* Array of slab pointers, index 0 is reserved. */
};

}

#endif
