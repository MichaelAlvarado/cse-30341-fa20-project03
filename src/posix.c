/* posix.c: POSIX API Implementation */

#include "malloc/counters.h"
#include "malloc/freelist.h"

#include <assert.h>
#include <string.h>

/**
 * Allocate specified amount memory.
 * @param   size    Amount of bytes to allocate.
 * @return  Pointer to the requested amount of memory.
 **/
void *malloc(size_t size) {
    // Initialize counters
    init_counters();

    // Handle empty size
    if (!size) {
        return NULL;
    }

    // TODO: Search free list for any available block with matching size

    Block *block = free_list_search(size);

    if(!block) { //No block available
        block = block_allocate(size);
    }
    else { //block available
        block = block_split(block, size);
        block = block_detach(block);
    }

    // Could not find free block or allocate a block, so just return NULL
    if (!block) {
        return NULL;
    }

    // Check if allocated block makes sense
    assert(block->capacity >= block->size);
    assert(block->size     == size);
    assert(block->next     == block);
    assert(block->prev     == block);

    // Update counters
    Counters[MALLOCS]++;
    Counters[REQUESTED] += size;

    // Return data address associated with block
    return block->data;
}

/**
 * Release previously allocated memory.
 * @param   ptr     Pointer to previously allocated memory.
 **/
void free(void *ptr) {
    if (!ptr) {
        return;
    }

    // Update counters
    Counters[FREES]++;

    // TODO: Try to release block, otherwise insert it into the free list
    Block *block = BLOCK_FROM_POINTER(ptr);

    if (!block_release(block)) {
        free_list_insert(block);
    }
}

/**
 * Allocate memory with specified number of elements and with each element set
 * to 0.
 * @param   nmemb   Number of elements.
 * @param   size    Size of each element.
 * @return  Pointer to requested amount of memory.
 **/
void *calloc(size_t nmemb, size_t size) {
    // TODO: Implement calloc
    Counters[CALLOCS]++;
    size_t total_size = nmemb * size;
    void *ptr = malloc(total_size);
    memset(ptr, 0, total_size);
    return ptr;
}

/**
 * Reallocate memory with specified size.
 * @param   ptr     Pointer to previously allocated memory.
 * @param   size    Amount of bytes to allocate.
 * @return  Pointer to requested amount of memory.
 **/
void *realloc(void *ptr, size_t size) {
    // TODO: Implement realloc
    Counters[REALLOCS]++;

    if (!ptr) {
        return malloc(size);
    }

    if (!size) {
        free(ptr);
        return NULL;
    }

    Block *block = BLOCK_FROM_POINTER(ptr);

    void *new_ptr;
    new_ptr = malloc(size);

    if (!new_ptr) {
        return NULL; // TODO: set errno on failure.
    }

    memcpy(new_ptr, ptr, block->size);
    free(ptr);
    return new_ptr;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
