/**
 * @file src/allocator/knr.c
 * @brief K&R-style allocator using a static memory arena.
 * @ref K&R C - 8.7: A Storage Allocator
 * @ref https://stackoverflow.com/q/13159564/15147156
 */

#include "core/memory.h"
#include "core/logger.h"
#include "allocator/knr.h"

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * Heap Configuration
 */

// Fixed array of 65536 units, each 16 bytes, for a total of 1 MB.
// Each allocation also needs at least one header unit.
static max_align_t buffer[HEAP_WORDS];

/**
 * Heap Definition
 */

static Heap heap = {
    .base = (uintptr_t) buffer,
    .end = (uintptr_t) buffer + sizeof(buffer),
    .current = (uintptr_t) buffer,
};

/**
 * Global Freelist Sentinel
 */

static FreeList base = {0}; /** sentinel value */
static FreeList* freelist = NULL; /* start of free list (head) */

/**
 * Private Functions
 */

static void allocator_freelist_init(void) {
    base.node.next = &base;
    base.node.size = 0;
    freelist = &base;
}

static bool coalesce_adjacent_neighbor(FreeList* a, FreeList* b) {
    return a + a->node.size == b;
}

static void merge_with_upper_neighbor(FreeList* a, FreeList* b) {
    a->node.size += b->node.next->node.size;
    a->node.next = b->node.next->node.next;
}

static void merge_with_lower_neighbor(FreeList* a, FreeList* b) {
    a->node.size += b->node.size;
    a->node.next = b->node.next;
}

/**
 * @brief Insert block into freelist and coalesce if adjacent
 */
static void allocator_freelist_insert(void* ptr) {
    FreeList* block = (FreeList*) ptr - 1; // move back to header
    FreeList* current = freelist;

    // Find insert point
    while (!(block > current && block < current->node.next)) {
        // Special case: At the start or end of circular list
        if (((current >= current->node.next) && (block > current)) || block < current->node.next) {
            break;
        }
        // Update the current pointer
        current = current->node.next;
    }

    // Try to merge with upper neighbor
    if (coalesce_adjacent_neighbor(block, current->node.next)) {
        merge_with_upper_neighbor(block, current);
    } else {
        block->node.next = current->node.next;
    }

    // Try to merge with lower neighbor
    if (coalesce_adjacent_neighbor(current, block)) {
        merge_with_lower_neighbor(current, block);
    } else {
        current->node.next = block;
    }

    // Update the free list
    freelist = current;
}

/**
 * @brief Allocate raw memory from heap
 */
static FreeList* allocator_freelist_heap_bump(size_t nunits) {
    size_t nbytes = nunits * HEADER_SIZE;
    if (heap.current + nbytes > heap.end) {
        return NULL;
    }

    FreeList* block = (FreeList*) heap.current;
    block->node.size = nunits;
    heap.current += nbytes;

    allocator_freelist_insert(block + 1);
    return freelist;
}

/**
 * Public Functions
 */

/**
 * @brief Allocate memory block
 */
void* allocator_freelist_malloc(size_t size) {
    uintptr_t payload_size = memory_aligned_size(size, MEMORY_ALIGNMENT);
    size_t nunits = (size + HEADER_SIZE - 1) / HEADER_SIZE + 1;

    LOG_DEBUG("size=%zu", size);
    LOG_DEBUG("payload_size=%zu", payload_size);
    LOG_DEBUG("nunits=%zu", nunits);

    if (NULL == freelist) {
        allocator_freelist_init();
    }

    FreeList* previous = freelist;
    FreeList* current = freelist->node.next;

    while (true) {
        if (current->node.size >= nunits) {
            if (current->node.size == nunits) {
                // Exact fit
                previous->node.next = current->node.next;
            } else {
                // Allocate tail end
                FreeList* alloc = current + current->node.size - nunits;
                alloc->node.size = nunits;
                current->node.size -= nunits;
                current = alloc;
            }
            freelist = previous;
            return (void*) (current + 1);
        }

        if (current == freelist) {
            if (NULL == allocator_freelist_heap_bump(nunits)) {
                return NULL; // Out of memory
            }
        }

        previous = current;
        current = current->node.next;
    }
}

/**
 * @brief Return memory block to freelist
 */
void allocator_freelist_free(void* ptr) {
    if (NULL == ptr) {
        return;
    }
    allocator_freelist_insert(ptr);
}

/**
 * @brief Returns the maximum 'size' parameter for which 
 *        allocator_freelist_malloc(size) will succeed.
 */
size_t allocator_freelist_max_alloc(void) {
    size_t units = HEAP_WORDS;
    if (units <= 1) return 0;
    // Each allocation needs at least one header unit
    size_t max_payload_units = units - 1;
    size_t max_payload_bytes = max_payload_units * sizeof(FreeList);
    // But this must be alignment-safe: round down to nearest multiple of MEMORY_ALIGNMENT
    max_payload_bytes = max_payload_bytes - (max_payload_bytes % MEMORY_ALIGNMENT);
    return max_payload_bytes;
}

/**
 * @brief Dump the contents of the freelist to the console
 */
void allocator_freelist_dump(void) {
    FreeList* current = freelist;
    printf("FreeList:\n");
    do {
        printf(
            "  Block at %p, size: %zu (%zu bytes)\n",
            (void*) current,
            current->node.size,
            current->node.size * sizeof(FreeList)
        );
        current = current->node.next;
    } while (current != freelist);
}
