/* block.c: Block Structure */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#include "malloc/block.h"
#include "malloc/counters.h"

/**
 * Allocate a new block on the heap using sbrk:
 *
 *  1. Determined aligned amount of memory to allocate.
 *  2. Allocate memory on the heap.
 *  3. Set allocage block properties.
 *
 * @param   size    Number of bytes to allocate.
 * @return  Pointer to data portion of newly allocate block.
 **/
Block *	block_allocate(size_t size) {
    // Allocate block
    intptr_t allocated = sizeof(Block) + ALIGN(size);
    Block *  block     = sbrk(allocated);
    if (block == SBRK_FAILURE) {
    	return NULL;
    }

    // Record block information
    block->capacity = ALIGN(size);
    block->size     = size;
    block->prev     = block;
    block->next     = block;

    // Update counters
    Counters[HEAP_SIZE] += allocated;
    Counters[BLOCKS]++;
    Counters[GROWS]++;
    return block;
}

/**
 * Attempt to release memory used by block to heap:
 *
 *  1. If the block is at the end of the heap.
 *  2. The block capacity meets the trim threshold.
 *
 * @param   block   Pointer to block to release.
 * @return  Whether or not the release completed successfully.
 **/
bool	block_release(Block *block) {
    // TODO: Implement block release
    bool result = false;
    size_t allocated = block->capacity + sizeof(Block);
    Block *currentBreak = sbrk(0);
    if (currentBreak != SBRK_FAILURE &&
            block->capacity >= TRIM_THRESHOLD &&
                (((char*)block) + allocated) == (char*)currentBreak) {
        block_detach(block);
        Block * block = sbrk(-allocated);
        if (block == SBRK_FAILURE) {
            return NULL;
        }
        Counters[BLOCKS]--;
        Counters[SHRINKS]++;
        Counters[HEAP_SIZE] -= allocated;
        result = true;
    }
    return result;
}

/**
 * Detach specified block from its neighbors.
 *
 * @param   block   Pointer to block to detach.
 * @return  Pointer to detached block.
 **/
Block * block_detach(Block *block) {
    // TODO: Detach block from neighbors by updating previous and next block
    block->prev->next = block->next;
    block->next->prev = block->prev;
    block->next = block;
    block->prev = block;
    return block;
}

/**
 * Attempt to merge source block into destination.
 *
 *  1. Compute end of destination and start of source.
 *
 *  2. If they both match, then merge source into destionation by giving the
 *  destination all of the memory allocated to source.
 *
 * @param   dst     Destination block we are merging into.
 * @param   src     Source block we are merging from.
 * @return  Whether or not the merge completed successfully.
 **/
bool	block_merge(Block *dst, Block *src) {
    // TODO: Implement block merge
    bool result = false;
    if ((((intptr_t)dst) + dst->capacity + sizeof(Block)) == ((intptr_t)src)) {
        dst->capacity = dst->capacity + src->capacity + sizeof(Block);
        Counters[MERGES]++;
        Counters[BLOCKS]--;
        result = true;
    }
    return result;
}

/**
 * Attempt to split block with the specified size:
 *
 *  1. Check if block capacity is sufficient for requested aligned size and
 *  header Block.
 *
 *  2. Split specified block into two blocks.
 *
 * @param   block   Pointer to block to split into two separate blocks.
 * @param   size    Desired size of the first block after split.
 * @return  Pointer to original block (regardless if it was split or not).
 **/
Block * block_split(Block *block, size_t size) {
    // TODO: Implement block split
    if (block->capacity >= (ALIGN(size) + sizeof(Block) + ALIGN(8))) {
        Block *newBlock = (Block*)(((intptr_t)block) + ALIGN(size) + sizeof(Block));
        newBlock->capacity = block->capacity - ALIGN(size) - sizeof(Block);
        newBlock->size = newBlock->capacity;
        newBlock->prev = block;
        newBlock->next = block->next;
        block->next->prev = newBlock;
        block->next = newBlock;
        block->capacity = ALIGN(size);
        block->size = size;
        Counters[SPLITS]++;
        Counters[BLOCKS]++;
    }
    return block;
}

/**
 * Sump a representation of a free block to a file
 *
 * @param block the block to dump
 * @param outFD the descriptor for the output file
 */
void    block_dump(Block *block, int outFD) {
    char buffer[BUFSIZ];
    fdprintf(outFD, buffer, "--->BLOCK 0x%0lx:\n", (long unsigned int)block);
    fdprintf(outFD, buffer, "\tCapacity: %ld\n", block->capacity);
    fdprintf(outFD, buffer, "\tSize: %ld\n", block->size);
    fdprintf(outFD, buffer, "\tNext: 0x%0lx\n", (long unsigned int)block->next);
    fdprintf(outFD, buffer, "\tPrev: 0x%0lx\n", (long unsigned int)block->prev);
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
