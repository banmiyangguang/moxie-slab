#include <assert.h>
#include <memory>
#include <iostream>

#include "Page.h"

moxie::Page::Page(size_t page_size, size_t chunk_size) {
    this->chunk_size = chunk_size;
    this->page_size = page_size;

    this->chunk_num = this->page_size / (this->chunk_size + sizeof(Page *));
    this->ptr = malloc(this->page_size);
    if (!ptr) {
        std::cout << "Alloc page ptr failed!" << std::endl;
        this->page_size = 0;
        return;
    }

    this->free_chunk_list = (void **)malloc(chunk_num * sizeof(Page *));
    if (this->free_chunk_list == nullptr) {
        this->page_size = 0;
        free(this->ptr);
        std::cout << "Alloc free chunk list failed!" << std::endl;
        return;
    }
    this->free_chunk_end = 0;
    this->free_chunk_start = 0;
}

moxie::Page::~Page() {
    if (this->free_chunk_list) {
        free(this->free_chunk_list);
    }
    if (this->ptr) {
        free(this->ptr);
    }
}

bool moxie::Page::reset(size_t chunk_size_new) {
    if (!this->ptr || !this->is_empty() || !moxie_list::out_of_list(&this->plist)) {
        return false;
    }

    if (this->page_size < (chunk_size_new + sizeof(Page *))) {
        return false;
    }

    this->chunk_size = chunk_size_new;
    this->chunk_num = this->page_size / (this->chunk_size + sizeof(Page *));
    this->free_chunk_list = (void **)realloc(this->free_chunk_list, this->chunk_num * sizeof(Page *));
    if (!this->free_chunk_list) {
        return false;
    }
    this->free_chunk_end = 0;
    this->free_chunk_start = 0;
    this->plist.next = this->plist.prev = nullptr;
    return true;
}

bool moxie::Page::vaild() const {
    return this->page_size != 0;
}

bool moxie::Page::is_empty() const {
    return this->free_chunk_end == this->free_chunk_start;
}

bool moxie::Page::is_full() const {
    return (this->free_chunk_start == this->chunk_num) && (this->free_chunk_end == 0);
}

void *moxie::Page::alloc_chunk() {
    Page **ptr = nullptr;
    if (this->free_chunk_start < this->chunk_num) {
        ptr = (Page **)((const char *)this->ptr + this->free_chunk_start * (this->chunk_size + sizeof(Page *)));    
        ++this->free_chunk_start;
    }

    if (this->free_chunk_end > 0) {
        ptr = (Page **)(this->free_chunk_list[this->free_chunk_end]);
        --this->free_chunk_end;
    }

    if (ptr) {
        ptr[0] = this;
        return ptr + 1;
    }

    return nullptr;
}

bool moxie::Page::free_chunk(void *chunk) {
    assert(PAGE_FROM_CHUNK(chunk) == this);
    this->free_chunk_list[this->free_chunk_end++] = chunk;
    return true;
}
