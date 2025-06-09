/**
 * @file src/allocator/knr.c
 * @brief K&R-style allocator using a static memory arena.
 * @ref K&R C - 8.7: A Storage Allocator
 * @ref https://stackoverflow.com/q/13159564
 * @ref https://stackoverflow.com/q/1119134
 * @ref https://stackoverflow.com/q/2513505
 */

#include "core/memory.h"
#include "allocator/knr.h"

#include <sys/mman.h>
#include <unistd.h>

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * Maximum amount of RAM to allocate (in bytes)
 */

static size_t MAX_RAM = 0;

/**
 * Global Freelist Sentinel
 */

static FreeList base = {0}; /** sentinel value */
static FreeList* freelist = NULL; /* start of free list (head) */

/**
 * Private Functions
 */

/**
 * @brief Returns the maximum allocatable memory based on system physical RAM,
 *        minus a fixed 1 GiB reserve to avoid exhausting system resources.
 *        Fallbacks to 4 GiB if RAM cannot be determined.
 */
static size_t system_max_ram(void) {
    int64_t pages = sysconf(_SC_PHYS_PAGES);
    int64_t page_size = sysconf(_SC_PAGE_SIZE);

    size_t max_ram;
    if (pages <= 0 || page_size <= 0) {
        max_ram = DSA_FALLBACK_MAX_RAM;
    } else {
        max_ram = (size_t) pages * (size_t) page_size;
    }

    // Never allow to allocate more than (fallback - reserve), but always at least 16 MiB.
    if (max_ram > DSA_RAM_RESERVE) {
        max_ram -= DSA_RAM_RESERVE;
    } else {
        max_ram = 16 * 1024 * 1024; // fallback minimum, 16 MiB
    }

    return max_ram;
}

static void allocator_freelist_init(void) {
    if (0 == MAX_RAM) {
        MAX_RAM = system_max_ram();
    }

    if (NULL == freelist) {
        base.next = &base;
        freelist = &base;
    }
}

static bool coalesce_adjacent_neighbor(FreeList* a, FreeList* b) {
    return a + a->size == b;
}

static void merge_with_upper_neighbor(FreeList* a, FreeList* b) {
    a->size += b->next->size;
    a->next = b->next->next;
}

static void merge_with_lower_neighbor(FreeList* a, FreeList* b) {
    a->size += b->size;
    a->next = b->next;
}

/**
 * @brief Insert block into freelist and coalesce if adjacent
 */
static void allocator_freelist_insert(void* ptr) {
    FreeList* block = (FreeList*) ptr - 1; // move back to header
    FreeList* current = freelist;

    // Find insert point
    while (!(block > current && block < current->next)) {
        // Special case: At the start or end of circular list
        if (((current >= current->next) && (block > current)) || block < current->next) {
            break;
        }
        // Update the current pointer
        current = current->next;
    }

    // Try to merge with upper neighbor
    if (coalesce_adjacent_neighbor(block, current->next)) {
        merge_with_upper_neighbor(block, current);
    } else {
        block->next = current->next;
    }

    // Try to merge with lower neighbor
    if (coalesce_adjacent_neighbor(current, block)) {
        merge_with_lower_neighbor(current, block);
    } else {
        current->next = block;
    }

    // Update the free list
    freelist = current;
}

/**
 * @brief Allocate raw memory from heap
 */
static FreeList* allocator_freelist_heap_bump(size_t nunits) {
    size_t nbytes = nunits * HEADER_SIZE;
    size_t page_size = (size_t) sysconf(_SC_PAGESIZE);

    // Round up to page size
    if (0 != nbytes % page_size) {
        nbytes += page_size - (nbytes % page_size);
    }

    void* address = mmap(NULL, nbytes, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (MAP_FAILED == address) {
        return NULL;
    }

    FreeList* block = (FreeList*) address;
    block->size = nbytes / HEADER_SIZE;
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
    allocator_freelist_init();
    if (size == 0 || size > MAX_RAM) {
        return NULL;
    }

    uintptr_t payload_size = memory_aligned_size(size, MEMORY_ALIGNMENT);
    size_t nunits = (payload_size + HEADER_SIZE - 1) / HEADER_SIZE + 1;

    FreeList* previous = freelist;
    FreeList* current = freelist->next;

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
            freelist = previous;
            return (void*) (current + 1);
        }

        if (current == freelist) {
            if (NULL == allocator_freelist_heap_bump(nunits)) {
                return NULL; // Out of memory
            }
        }

        previous = current;
        current = current->next;
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
 * @brief Dump the contents of the freelist to the console
 */
void allocator_freelist_dump(void) {
    FreeList* current = freelist;
    printf("FreeList:\n");
    do {
        printf(
            "  Block at %p, size: %zu (%zu bytes)\n",
            (void*) current,
            current->size,
            current->size * sizeof(FreeList)
        );
        current = current->next;
    } while (current != freelist);
}
