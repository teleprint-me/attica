/**
 * Copyright © 2023 Austin Berrio
 *
 * @file include/core/memory.h
 * @brief Utility functions for memory alignment, padding, and allocation.
 *
 * Provides helper functions to:
 * - Check power-of-two properties
 * - Determine alignment of addresses or sizes
 * - Calculate padding and aligned sizes
 * - Allocate aligned memory blocks with posix_memalign
 */

#ifndef DSA_MEMORY_H
#define DSA_MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdalign.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include <assert.h>
#include <malloc.h>

#ifndef alignof
    #define alignof _Alignof
#endif

/**
 * @brief Default memory alignment (8 bytes).
 */
#define MEMORY_ALIGNMENT (sizeof(max_align_t))

/**
 * @name Alignment Utilities
 * @{
 */

/**
 * @brief Computes x modulo y, optimized for when y is a power of two.
 *
 * @param x The value (or address) to compute offset for.
 * @param y The alignment boundary; must be greater than zero.
 * @return The remainder of x divided by y (x % y).
 */
uintptr_t memory_bitwise_offset(uintptr_t x, uintptr_t y);

/**
 * @brief Checks if a value is a nonzero power of two.
 *
 * @param x Value to test.
 * @return true if x is a power of two and not zero, false otherwise.
 */
bool memory_is_power_of_two(uintptr_t x);

/**
 * @brief Checks if a value or address is aligned to the given boundary.
 *
 * @param x The value or address to check.
 * @param alignment Alignment boundary; must be a power of two.
 * @return true if x is aligned to alignment, false otherwise.
 */
bool memory_is_aligned(uintptr_t x, uintptr_t alignment);

/**
 * @brief Rounds up an address to the next aligned boundary.
 *
 * If the address is already aligned, it is returned unchanged.
 *
 * @param address Starting address.
 * @param alignment Alignment boundary; must be a power of two.
 * @return The next aligned address >= address.
 */
uintptr_t memory_next_aligned_address(uintptr_t address, uintptr_t alignment);

/**
 * @brief Computes the number of bytes needed to pad an address up to alignment.
 *
 * @param address The current address.
 * @param alignment Alignment boundary; must be a power of two.
 * @return Number of bytes to add to address to align it.
 */
size_t memory_padding_needed(uintptr_t address, size_t alignment);

/**
 * @brief Rounds a size up to the nearest multiple of alignment.
 *
 * @param x Size (or address) to round.
 * @param alignment Alignment boundary; must be a power of two.
 * @return Rounded size, the smallest multiple of alignment >= x.
 */
uintptr_t memory_aligned_size(uintptr_t x, uintptr_t alignment);

/** @} */

/**
 * @name Aligned Memory Allocation
 * @{
 */

/**
 * @brief Allocates size bytes of memory aligned to alignment bytes.
 *
 * Uses posix_memalign internally.
 * The returned pointer must be freed with free().
 *
 * @param size Number of bytes to allocate.
 * @param alignment Alignment boundary; must be a power of two and >= sizeof(void *).
 * @return Pointer to allocated memory on success, NULL on failure.
 */
void* memory_aligned_alloc(size_t size, size_t alignment);

/**
 * @brief Allocates zero-initialized memory for an array with alignment.
 *
 * Equivalent to calloc but aligned.
 *
 * @param n Number of elements.
 * @param size Size of each element.
 * @param alignment Alignment boundary; must be a power of two.
 * @return Pointer to zeroed memory on success, NULL on failure.
 */
void* memory_aligned_calloc(size_t n, size_t size, size_t alignment);

/**
 * @brief Reallocates aligned memory to a new size and alignment.
 *
 * This performs the equivalent of a realloc, but guarantees alignment.
 * If the original pointer is NULL, it behaves like memory_aligned_alloc.
 * If the new size is zero, it frees the memory and returns NULL.
 *
 * The original pointer must have been allocated with memory_aligned_alloc or memory_aligned_calloc.
 *
 * @param ptr Pointer to the memory block to reallocate.
 * @param old_size Size of the original allocation in bytes.
 * @param new_size New size in bytes.
 * @param alignment Desired memory alignment (must be power of two).
 * @return Pointer to newly allocated memory, or NULL on failure.
 */
void* memory_aligned_realloc(void* ptr, size_t old_size, size_t new_size, size_t alignment);

/**
 * @brief Frees aligned memory.
 *
 * This function is equivalent to free, but works with aligned memory.
 * 
 * @param ptr Pointer to the memory block.
 */
void memory_aligned_free(void* ptr);

/** @} */

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DSA_MEMORY_H
