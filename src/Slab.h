#ifndef MOXIE_SLAB_H
#define MOXIE_SLAB_H
#include <stdlib.h>

#include <Page.h>

namespace moxie {

class Slab {
public:
    friend class MemCache;
    Slab(size_t chunk_size, size_t page_size, int pre_alloc);
    ~Slab();
    void *slab_alloc_chunk();
    bool slab_free_chunk(void *ptr);
    bool has_enough_memory() const;
    bool slab_new_page();
    Page *pop_empty_page();
    bool push_empty_page(Page *page);
private:
    Page* insert_to_list_from_head(Page *list, Page *page);
    Page* remove_from_list(Page *list, Page *page);
    Page* pick_page_from_list(Page *&list);
private:
    size_t page_size;
    size_t chunk_size;
    size_t page_total;
    size_t free_chunk_count;
    moxie_list::mlist* empty_pages;
    moxie_list::mlist* partial_pages;
    moxie_list::mlist* full_pages;
};

}

#define GET_SLAB_FROM_CHUNK(ptr) (((Slab *)ptr) - 1)
#define CHECK_SLAB_MAGIC_NUMBER(ptr) (SLAB_MAGIC_NUMBER == ptr->magic_number)

#endif
