/**
 * @file src/allocator/knr.c
 * @brief Allocator as described in Kerrigen and Ritchies C.
 * @todo This is currently ear marked.
 * @ref K&R C - 8.7 Example - A Storage Allocator
 * @ref https://stackoverflow.com/q/13159564/15147156
 */

#include "core/memory.h"

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

/**
 * Define the heap
 */

#define HEAP_WORDS (1024 * 1024 / sizeof(size_t)) /** Heap size in words */
static size_t buffer[HEAP_WORDS]; /** Heap buffer */

typedef struct Heap {
    uintptr_t base; /** Base of the heap */
    uintptr_t end; /** End of the heap */
    uintptr_t current; /** Current position in the heap */
} Heap;

static Heap heap = {
    .base = (uintptr_t) buffer,
    .end = (uintptr_t) buffer + sizeof(buffer),
    .current = (uintptr_t) buffer,
};

/**
 * Define the header
 */

typedef union Header Header; /** Forward declaration for Node */

typedef struct Node {
    Header* next; /* next block if on free list */
    size_t size; /* size of this block */
} Node;

typedef union Header {
    Node node; /** block header */
    size_t alignment; /* force alignment of blocks */
} Header;

static Header base; /** sentinel value */
static Header* freelist = NULL; /* start of free list (head) */

/**
 * Private helper functions
 */

static bool adjacent(Header* a, Header* b) {
    return a + a->node.size == b;
}

static void merge_with_next(Header* a, Header* b) {
    a->node.size += b->node.next->node.size;
    a->node.next = b->node.next->node.next;
}

static void merge_with_previous(Header* a, Header* b) {
    a->node.size += b->node.size;
    a->node.next = b->node.next;
}

/**
 * put block in free list
 */
void allocator_freelist_insert(void* ptr) {
    Header* block = (Header*) ptr - 1; // move back to header
    Header* current = freelist;

    // Find where block fits between current and current->next
    while (!(block > current && block < current->node.next)) {
        // Special case: At the start or end of circular list
        if (current >= current->node.next && block > current || block < current->node.next) {
            break;
        }
        // Update the current pointer
        current = current->node.next;
    }

    // Coalesce with upper neighbor if adjacent
    if (adjacent(block, current->node.next)) {
        merge_with_next(block, current);
    } else {
        block->node.next = current->node.next;
    }

    // Coalesce with lower neighbor if adjacent
    if (adjacent(current, block)) {
        merge_with_previous(current, block);
    } else {
        current->node.next = block;
    }

    // Update the free list
    freelist = current;
}

/**
 * morecore: ask system for more memory
 *
 * @note nunits should always include the header (e.g., n = payload_units + 1)
 * @note maybe rename this to bump? not sure yet.
 */
static Header* allocator_freelist_alloc(size_t nunits) {
    size_t nbytes = nunits * sizeof(Header);
    if (heap.current + nbytes > heap.end) {
        return NULL;
    }

    Header* block = (Header*) heap.current;
    block->node.size = nunits;
    heap.current += nbytes;

    // Add to free list
    allocator_freelist_insert(block);
    return block;
}

/**
 * malloc: general-purpose storage allocator
 */
void* allocator_freelist_malloc(size_t size) {
    Header* current = NULL;
    Header* previous = NULL;
    size_t nunits = (size + sizeof(Header) - 1) / sizeof(Header) + 1;

    if (freelist == NULL) {
        static Header base;
        base.node.next = &base;
        base.node.size = 0;
        freelist = &base;
    }

    previous = freelist;
    while (true) {
        current = previous->node.next;

        if (current->node.size >= nunits) {
            if (current->node.size == nunits) {
                // Exact fit
                previous->node.next = current->node.next;
            } else {
                // Allocate tail end
                Header* alloc = current + current->node.size - nunits;
                alloc->node.size = nunits;
                current->node.size -= nunits;
                current = alloc;
            }
            freelist = previous;
            return (void*) (current + 1);
        }

        if (current == freelist) {
            // No fit found, try to expand heap
            Header* new_block = allocator_freelist_alloc(nunits);
            if (!new_block) {
                return NULL;
            }
            // Start search again
            return allocator_freelist_malloc(size);
        }

        previous = current;
        current = current->node.next;
    }
}
