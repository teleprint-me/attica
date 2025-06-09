/**
 * @file include/allocator/knr.h
 * @brief K&R-style allocator using a static memory arena.
 * @ref K&R C - 8.7: A Storage Allocator
 * @ref https://stackoverflow.com/q/13159564
 * @ref https://stackoverflow.com/q/1119134
 * @ref https://stackoverflow.com/q/2513505
 */

#ifndef DSA_ALLOCATOR_KNR_H
#define DSA_ALLOCATOR_KNR_H

#include <stddef.h>

#ifndef DSA_FALLBACK_MAX_RAM
    #define DSA_FALLBACK_MAX_RAM ((size_t) 1 << 32) // 4 GiB
#endif

#ifndef DSA_RAM_RESERVE
    #define DSA_RAM_RESERVE ((size_t) 1 << 30) // 1 GiB
#endif

/**
 * FreeList Block
 */

typedef struct FreeList {
    struct FreeList* next; /* next block if on free list */
    size_t size; /* size of this block */
} FreeList;

#define HEADER_SIZE sizeof(FreeList)

void* allocator_freelist_malloc(size_t size);
void allocator_freelist_free(void* ptr);
void allocator_freelist_dump(void);

#endif // DSA_ALLOCATOR_KNR_H
