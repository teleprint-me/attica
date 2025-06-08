/**
 * @file include/allocator/knr.h
 * @brief K&R-style allocator using a static memory arena.
 * @ref K&R C - 8.7: A Storage Allocator
 * @ref https://stackoverflow.com/q/13159564/15147156
 */

#ifndef DSA_ALLOCATOR_KNR_H
#define DSA_ALLOCATOR_KNR_H

#include <stddef.h>

void* allocator_freelist_malloc(size_t size);
void allocator_freelist_free(void* ptr);
void allocator_freelist_dump(void);

#endif // DSA_ALLOCATOR_KNR_H
