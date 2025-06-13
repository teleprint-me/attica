/**
 * Copyright © 2023 Austin Berrio
 *
 * @file src/allocator/knr.c
 * @brief K&R-style allocator using a static memory arena.
 *
 * @ref K&R C - 8.7: A Storage Allocator
 * @ref https://stackoverflow.com/q/13159564
 * @ref https://stackoverflow.com/q/1119134
 * @ref https://stackoverflow.com/q/2513505
 */

#include "core/memory.h"
#include "allocator/knr.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * @name Private
 * {@
 */

typedef struct FreeList {
    struct FreeList* next; /* next block if on free list */
    size_t size; /* size of this block */
} FreeList;

/**
 * Global Freelist Sentinel
 */
static FreeList* base = NULL; /** sentinel value */
static FreeList* head = NULL; /* start of free list (head) */

/**
 * @brief Insert block into freelist and coalesce if adjacent
 */

static bool freelist_block_is_neighbor(FreeList* a, FreeList* b) {
    return a + a->size == b;
}

static void freelist_block_merge_upward(FreeList* a, FreeList* b) {
    a->size += b->next->size;
    a->next = b->next->next;
}

static void freelist_block_merge_downward(FreeList* a, FreeList* b) {
    a->size += b->size;
    a->next = b->next;
}

static void freelist_block_insert(void* ptr) {
    if (NULL == ptr) {
        return;
    }

    FreeList* block = ((FreeList*) ptr) - 1; // get header
    FreeList* current = head;

    while (!(block > current && block < current->next)) {
        if (current >= current->next && (block > current || block < current->next)) {
            break; // wrapped around
        }
        current = current->next;
    }

    // Merge with upper neighbor if possible
    if (freelist_block_is_neighbor(block, current->next)) {
        freelist_block_merge_upward(block, current);
    } else {
        block->next = current->next;
    }

    // Merge with lower neighbor if possible
    if (freelist_block_is_neighbor(current, block)) {
        freelist_block_merge_downward(current, block);
    } else {
        current->next = block;
    }

    head = current;
}

/**
 * @brief Allocate raw memory from heap
 */
static FreeList* freelist_block_new(size_t nunits) {
    size_t nbytes = nunits * sizeof(FreeList);

    FreeList* block = (FreeList*) memory_alloc(nbytes, alignof(FreeList));
    if (NULL == block) {
        return NULL;
    }

    block->size = nbytes / sizeof(FreeList);
    freelist_block_insert(block + 1);
    return block;
}

/** @} */

/**
 * @name Public
 * {@
 */

/**
 * @brief Initialize the global free list.
 */
bool freelist_initialize(void) {
    if (NULL == base) {
        base = (FreeList*) memory_alloc(sizeof(FreeList), alignof(FreeList));
        if (NULL == base) {
            return false;
        }
    }

    if (NULL == head) {
        base->next = base;
        base->size = 0;
        head = base;
    }

    return true;
}

/**
 * @brief Destroy the global free list and free all blocks.
 */
bool freelist_terminate(void) {
    if (NULL == base) {
        return false;
    }

    FreeList* current = base->next;
    while (current != base) {
        FreeList* next = current->next;
        memory_free(current);
        current = next;
    }

    memory_free(base);
    base = NULL;
    head = NULL;
    return true;
}

void* freelist_malloc(size_t size) {
    if (size == 0 || size > memory_ram_free()) {
        return NULL;
    }

    if (!freelist_initialize()) {
        return NULL;
    }

    uintptr_t payload_size = memory_align_up(size, alignof(FreeList));
    size_t nunits = (payload_size + sizeof(FreeList) - 1) / sizeof(FreeList) + 1;

    FreeList* previous = head;
    FreeList* current = head->next;

    while (true) {
        if (current->size >= nunits) {
            if (current->size == nunits) {
                // Exact fit
                previous->next = current->next;
            } else {
                // Allocate tail end
                FreeList* alloc = current + current->size - nunits;
                alloc->size = nunits;
                current->size -= nunits;
                current = alloc;
            }
            head = previous;
            return (void*) (current + 1);
        }

        if (current == head) {
            if (NULL == freelist_block_new(nunits)) {
                return NULL; // Out of memory
            }
        }

        previous = current;
        current = current->next;
    }
}

void freelist_free(void* ptr) {
    if (NULL == ptr) {
        return;
    }
    freelist_block_insert(ptr);
}

void freelist_dump(void) {
    printf("freelist:\n");
    if (!base) {
        printf("  [uninitialized]\n");
        return;
    }

    FreeList* current = base->next;
    while (current != base) {
        printf(
            "  block: %p | size: %zu | next: %p\n",
            (void*) current,
            current->size,
            (void*) current->next
        );
        current = current->next;
    }
}

/** @} */

/// @brief Usage Example
int main(void) {
    if (!freelist_initialize()) {
        fprintf(stderr, "Failed to initialize freelist state\n");
        return EXIT_FAILURE;
    }

    int* x = (int*) freelist_malloc(sizeof(int));
    if (!x) {
        fprintf(stderr, "Failed to allocate int\n");
        return EXIT_FAILURE;
    }
    *x = 5;
    printf("Value of x: %d\n", *x);
    freelist_free(x);

    if (!freelist_terminate()) {
        fprintf(stderr, "Failed to terminate freelist state\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
