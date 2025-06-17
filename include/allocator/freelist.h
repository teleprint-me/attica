/**
 * Copyright © 2023 Austin Berrio
 *
 * @file include/allocator/knr.c
 * @brief K&R-style allocator using a static memory arena.
 *
 * @ref K&R C - 8.7: A Storage Allocator
 * @ref https://stackoverflow.com/q/13159564
 * @ref https://stackoverflow.com/q/1119134
 * @ref https://stackoverflow.com/q/2513505
 */

#ifndef DSA_ALLOCATOR_KNR_H
#define DSA_ALLOCATOR_KNR_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the freelist allocator.
 *
 * Must be called before any allocation.
 * Safe to call multiple times (no-op after first init).
 */
bool freelist_initialize(void);

/**
 * @brief Destroy all freelist state.
 *
 * Frees any resources owned by the allocator.
 * After this, no allocations are valid.
 */
bool freelist_terminate(void);

/**
 * @brief Allocate a memory block using the freelist allocator.
 *
 * @param size Number of bytes to allocate.
 * @return Pointer to memory block, or NULL on failure.
 */
void* freelist_malloc(size_t size);

/**
 * @brief Return a previously allocated block to the freelist.
 *
 * @param ptr Pointer to memory block previously allocated with allocator_freelist_malloc().
 */
void freelist_free(void* ptr);

/**
 * @brief Dump the current state of the freelist (for debugging).
 */
void freelist_dump(void);

#ifdef __cplusplus
}
#endif

#endif // DSA_ALLOCATOR_KNR_H
