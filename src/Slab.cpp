/* -*- Mode: C; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Slabs memory allocation, based on powers-of-N. Slabs are up to 1MB in size
 * and are divided into chunks. The chunk sizes start off at the size of the
 * "item" structure plus space for a small key and value. They increase by
 * a multiplier factor from there, up to half the maximum slab size. The last
 * slab size is always 1MB, since that's the maximum item size allowed by the
 * memcached protocol.
 *
 * $Id$
 */
#include <sys/types.h>
#include <stdlib.h>
#include <stddef.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <memory>

#include "Slab.h"
#include "Items.h"


moxie::Slab::Slab(size_t chunk_size, size_t page_size, int pre_alloc) {
    if (chunk_size % CHUNK_ALIGN_BYTES) {
        chunk_size += CHUNK_ALIGN_BYTES - (chunk_size % CHUNK_ALIGN_BYTES);
    }
    
    if (page_size % PAGE_ALIGN_BYTES) {
        page_size += PAGE_ALIGN_BYTES - (page_size % PAGE_ALIGN_BYTES);
    }

    this->chunk_size = chunk_size;
    this->page_size = page_size;
    this->page_total = 0;
    this->empty_pages = new (std::nothrow) moxie_list::mlist;
    this->full_pages = new (std::nothrow) moxie_list::mlist;
    this->partial_pages = new (std::nothrow) moxie_list::mlist;

    if (pre_alloc) {
       slab_new_page();
    }
}

moxie::Slab::~Slab() {
    moxie_list::mlist *head = this->empty_pages->next;
    while (head) {
        Page *page = PAGE_FROM_PLIST(head);
        head = moxie_list::remove_from_list_ret_next(&page->plist);
        delete page;
        page = nullptr;
    }
    head = this->full_pages->next;
    while (head) {
        Page *page = PAGE_FROM_PLIST(head);
        head = moxie_list::remove_from_list_ret_next(&page->plist);
        delete page;
        page = nullptr;
    }
    head = this->partial_pages->next;
    while (head) {
        Page *page = PAGE_FROM_PLIST(head);
        head = moxie_list::remove_from_list_ret_next(&page->plist);
        delete page;
        page = nullptr;
    }
    delete this->empty_pages;
    delete this->full_pages;
    delete this->partial_pages;
}

bool moxie::Slab::slab_new_page() {
    Page *page = new (std::nothrow) Page(this->page_size, this->chunk_size);
    if (!page) {
        return false;
    }
    if (!moxie_list::insert_to_list_after_pnode(this->empty_pages, &page->plist)) {
        delete page;
        return false;
    }
    this->free_chunk_count += page->chunk_num;
    ++this->page_total;
    return true;
}

bool moxie::Slab::has_enough_memory() const {
    return this->free_chunk_count > 0;
}

void *moxie::Slab::slab_alloc_chunk() {
    void *ret = nullptr;
    moxie_list::mlist *partial = this->partial_pages->next;
    while (partial) {
        Page *page = PAGE_FROM_PLIST(partial);
        if (page->is_full()) {
            partial = moxie_list::remove_from_list_ret_next(&page->plist);
            moxie_list::insert_to_list_after_pnode(this->full_pages, &page->plist);
            continue;
        }
        ret = page->alloc_chunk();
        assert(ret);
        if (page->is_full()) {
            moxie_list::remove_from_list(&page->plist);
            moxie_list::insert_to_list_after_pnode(this->full_pages, &page->plist);
        }
        break;
    }

    if (!this->partial_pages->next) {
        partial = this->empty_pages->next;
        if (partial) {
            Page *page = PAGE_FROM_PLIST(partial);
            assert(page->is_empty());
            ret = page->alloc_chunk();
            assert(ret);
            moxie_list::remove_from_list(&page->plist);
            if (page->is_full()) {
                moxie_list::insert_to_list_after_pnode(this->full_pages, &page->plist);
            } else {
                moxie_list::insert_to_list_after_pnode(this->partial_pages, &page->plist);
            }
        }
    }

    if (ret) {
        assert(this->free_chunk_count > 0);
        this->free_chunk_count--;
    }

    return ret;
}

moxie::Page *moxie::Slab::pop_empty_page() {
    moxie_list::mlist *elist = this->empty_pages->next;
    if (!elist) {
        return nullptr;
    }
    Page *page = PAGE_FROM_PLIST(elist);
    moxie_list::remove_from_list(&page->plist);
    assert(page->is_empty());
    assert(this->free_chunk_count >= page->chunk_num);
    this->free_chunk_count -= page->chunk_num;
    return page;
}

bool moxie::Slab::push_empty_page(Page *page) {
    if (!page || !page->is_empty()) {
        return false;
    }

    if (!page->reset(this->chunk_size)) {
        return false;
    }

    return moxie_list::insert_to_list_after_pnode(this->empty_pages, &page->plist);
}

bool moxie::Slab::slab_free_chunk(void *ptr) {
    if (!ptr) {
        return false;
    }
    Page *page = PAGE_FROM_CHUNK(ptr);
    if (page->is_full()) { // remove from full pages list
        moxie_list::remove_from_list(&page->plist);
    }
    page->free_chunk(ptr);
    if (page->is_empty()) {
        moxie_list::remove_from_list(&page->plist);
        moxie_list::insert_to_list_after_pnode(this->empty_pages, &page->plist);
    }

    if (moxie_list::out_of_list(&page->plist)) {
        moxie_list::insert_to_list_after_pnode(this->partial_pages, &page->plist);
    } 
    this->free_chunk_count++;
    return true;
}
