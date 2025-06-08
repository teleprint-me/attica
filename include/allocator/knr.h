/**
 * @file include/allocator/knr.h
 * @brief K&R-style allocator using a static memory arena.
 * @ref K&R C - 8.7: A Storage Allocator
 * @ref https://stackoverflow.com/q/13159564/15147156
 */

#ifndef DSA_ALLOCATOR_KNR_H
#define DSA_ALLOCATOR_KNR_H

#include <stddef.h>

/**
 * Heap Configuration
 */

#define HEAP_BYTES (1024 * 1024) // 1048576 bytes
#define HEAP_WORDS (HEAP_BYTES / MEMORY_ALIGNMENT) // 65536 units (if 16-byte alignment)

/**
 * Heap Definition
 */

typedef struct Heap {
    uintptr_t base; /** Base of the heap */
    uintptr_t end; /** End of the heap */
    uintptr_t current; /** Current position in the heap */
} Heap;

/**
 * FreeList Block
 */

typedef union FreeList FreeList; /** Forward declaration for Node */

typedef struct Node {
    FreeList* next; /* next block if on free list */
    size_t size; /* size of this block */
} Node;

typedef union FreeList {
    Node node; /** block header */
    size_t alignment; /* force alignment of blocks */
} FreeList;

#define HEADER_SIZE sizeof(FreeList)

void* allocator_freelist_malloc(size_t size);
void allocator_freelist_free(void* ptr);
size_t allocator_freelist_max_alloc(void);
void allocator_freelist_dump(void);

#endif // DSA_ALLOCATOR_KNR_H
