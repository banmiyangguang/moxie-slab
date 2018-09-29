#include <iostream>
#include <assert.h>
#include <memory>
#include <string.h>

#include "MemCache.h"

/*
 * Determines the chunk sizes and initializes the slab class descriptors
 * accordingly.
 */
moxie::MemCache::MemCache(size_t base_chunk_size, 
        double factor, 
        size_t power_largest, 
        size_t power_block, 
        size_t mem_limit, 
        int pre_alloc) {

    size_t i;
    size_t chunk_size;
    if (base_chunk_size <= CHUNK_ALIGN_BYTES || 
            power_largest <= 0 || 
            power_block <= CHUNK_ALIGN_BYTES ||
            power_block < base_chunk_size ||
            mem_limit <= CHUNK_ALIGN_BYTES) {
        assert(false);
    }
    
    this->slabclass = (Slab **)calloc(power_largest, sizeof(Slab *));
    if (NULL == slabclass) {
        assert(false);
    }

    this->mem_limit = mem_limit;
    this->mem_malloced = 0;
    this->factor = factor;
    this->power_block = power_block;
    this->base_chunk_size = base_chunk_size;
    this->power_smallest = 1;
    this->slabclass = slabclass;
    
    for (i = 1, chunk_size = base_chunk_size; i < power_largest; ++i) {
        if (chunk_size > power_block) {
            break;
        }
        slabclass[i] = new (std::nothrow) Slab(chunk_size, power_block, pre_alloc);
        printf("slab class %3lu: chunk size %6lu\n", i, slabclass[i]->chunk_size);
        chunk_size *= factor;
    }
    
    this->power_largest = i;
    this->mem_malloced = this->power_largest;
}

moxie::MemCache::~MemCache() {
    for (size_t i = this->power_smallest; i < this->power_largest; ++i) {
        delete this->slabclass[i];
    }
    free(this->slabclass);
}

moxie::Item *moxie::MemCache::create_item(const char *key, size_t keylen, rel_time_t exptime, const char* data, size_t nbytes) {
    if (!key || !data) {
        return nullptr;
    }
    int ntotal = item_make_header(keylen, nbytes);

    size_t slab_id = mem_cache_clsid(ntotal);
    if (slab_id == 0) {
        return nullptr;
    }
    void *mem = mem_cache_alloc(ntotal, slab_id);
    if (mem == nullptr) {
        return nullptr;
    }
    Item *it = new (mem) Item(this);
    assert(it->slabs_clsid == 0);

    it->slabs_clsid = slab_id;

    it->h_next = nullptr;
    it->h_prev = nullptr;
    it->refcount = 0;
    it->nkey = keylen;
    it->setflags(ITEM_ALLOC);
    it->nbytes = nbytes;
    memcpy(it->ITEM_key(), key, it->nkey);
    memcpy(it->ITEM_data(), data, it->nbytes);
    it->exptime = exptime;
    return it;
}

void moxie::MemCache::recycle_item(moxie::Item *it) {
    unsigned int ntotal = it->ITEM_ntotal();
    assert(it->refcount == 0);
    it->slabs_clsid = 0;
    it->clearflags(ITEM_ALLOC);
    mem_cache_free(it, ntotal);
}

void *moxie::MemCache::mem_cache_alloc(size_t size, size_t slab_id) {
    void *ptr;
    Slab* slab = nullptr;
    size_t org_page_total;
    
    if (size <= 0 || slab_id <= 0) {
        return nullptr;
    }

    slab = this->slabclass[slab_id];
    org_page_total = slab->page_total;
    ptr = slab->slab_alloc_chunk();

    if (nullptr == ptr) {
        assert(!slab->has_enough_memory());
        if ((this->mem_malloced >= this->mem_limit) && (!addjust_pages_to_slab(slab_id))) {
            return nullptr;
        }
        if (!slab->slab_new_page()) {
            return nullptr;
        }
        ptr = slab->slab_alloc_chunk();
    }

    if (org_page_total < slab->page_total) {
        this->mem_malloced += slab->page_size;
    }

    return ptr;
}

bool moxie::MemCache::mem_cache_free(int slab_id, void *ptr) {
    if (slab_id <= 0 || !ptr) {
        return false;
    }
    return this->slabclass[slab_id]->slab_free_chunk(ptr);
}

bool moxie::MemCache::mem_cache_free(void *ptr, size_t size) {
    size_t slab_id = mem_cache_clsid(size);
    if (slab_id <= 0 || !ptr) {
        return false;
    }
    return mem_cache_free(slab_id, ptr);
}

bool moxie::MemCache::addjust_pages_to_slab(size_t slab_id) {
    for (size_t i = this->power_smallest; i <= this->power_largest; ++i) {
        if (i == slab_id) {
            continue;
        }
        Page *page = this->slabclass[i]->pop_empty_page();
        if (page) {
            if (this->slabclass[slab_id]->push_empty_page(page)) {
                return true;
            }
            this->slabclass[i]->push_empty_page(page);
        }
    }
    return false;
}

size_t moxie::MemCache::mem_cache_clsid(size_t size) {
    size_t res;
    if(0 == size) {
        return 0;
    }

    res = this->power_smallest;
    while (size > this->slabclass[res]->chunk_size) {
        if (res >= this->power_largest) {
            return 0;
        }
        ++res;
    }
    return res;
}