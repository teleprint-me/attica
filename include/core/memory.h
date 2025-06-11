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
 * @brief Default memory alignment.
 */
#define MEMORY_ALIGNMENT (alignof(max_align_t))

/**
 * @name Alignment Utilities
 * @{
 */

/**
 * @brief Check if a value is a power of two (and not zero).
 *
 * @param value The value to check.
 * @return true if value is a power of two and not zero, false otherwise.
 */
bool memory_is_power_of_two(uintptr_t value);

/**
 * @brief Returns the offset of a value within the given alignment.
 *
 * For example, for alignment = 8 and value = 14, returns 6.
 *
 * @param value The value to check.
 * @param alignment The alignment boundary (must be a power of two).
 * @return The offset of value within alignment.
 */
uintptr_t memory_alignment_offset(uintptr_t value, uintptr_t alignment);

/**
 * @brief Returns true if value is aligned to the given alignment.
 *
 * @param value The value to check.
 * @param alignment The alignment boundary (must be a power of two).
 * @return true if value is aligned, false otherwise.
 */
bool memory_is_aligned(uintptr_t value, uintptr_t alignment);

/**
 * @brief Aligns value *up* to the next multiple of alignment.
 *
 * If value is already aligned, returns value unchanged.
 *
 * @param value The value to align.
 * @param alignment The alignment boundary (must be a power of two).
 * @return The aligned value rounded up.
 */
uintptr_t memory_align_up(uintptr_t value, uintptr_t alignment);

/**
 * @brief Aligns value *down* to the previous multiple of alignment.
 *
 * If value is already aligned, returns value unchanged.
 *
 * @param value The value to align.
 * @param alignment The alignment boundary (must be a power of two).
 * @return The aligned value rounded down.
 */
uintptr_t memory_align_down(uintptr_t value, uintptr_t alignment);

/**
 * @brief Returns the number of padding bytes needed to align address up to alignment.
 *
 * If already aligned, returns 0.
 *
 * @param address The address to check.
 * @param alignment The alignment boundary (must be a power of two).
 * @return Number of bytes padding needed.
 */
size_t memory_padding_needed(uintptr_t address, size_t alignment);

/**
 * @brief Returns the minimal count of objects (of object_size) required to cover size,
 *        after rounding size up to the given alignment.
 *
 * For example, allocating N objects of size object_size with overall buffer size >= size (aligned).
 *
 * @param nbytes The required size in bytes.
 * @param nsize The size of each object.
 * @param alignment The alignment boundary (must be a power of two).
 * @return The minimal number of objects required.
 */
uintptr_t memory_object_count(uintptr_t nbytes, uintptr_t nsize, uintptr_t alignment);

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
