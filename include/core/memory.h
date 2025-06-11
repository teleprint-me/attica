/**
 * Copyright © 2023 Austin Berrio
 *
 * @file include/core/memory.h
 * @brief Utility functions for memory alignment, padding, and aligned allocation.
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

#ifndef alignof
    #define alignof _Alignof
#endif

/**
 * @brief Default memory alignment.
 */
#define MEMORY_ALIGNMENT (alignof(max_align_t))

/**
 * @brief Default maximum fallback memory size in bytes.
 */
#ifndef MEMORY_MAX_FALLBACK
    #define MEMORY_MAX_FALLBACK ((size_t) 1 << 32) // 4 GiB
#endif

/**
 * @brief Default maximum reserve memory size in bytes.
 */
#ifndef MEMORY_MAX_RESERVE
    #define MEMORY_MAX_RESERVE ((size_t) 1 << 30) // 1 GiB
#endif

/**
 * @name RAM Utilities
 * @{
 */

/**
 * @brief Returns the maximum allocatable RAM size in bytes.
 *
 * Takes into account system physical memory and reserves a fixed amount.
 *
 * @return Maximum allocatable RAM size in bytes.
 */
size_t memory_ram_max(void);

/**
 * @brief Returns the total physical RAM size in bytes.
 *
 * @return Total physical RAM size in bytes.
 */
size_t memory_ram_total(void);

/**
 * @brief Returns the amount of free RAM in bytes.
 *
 * @return Amount of free RAM in bytes.
 */
size_t memory_ram_free(void);

/** @} */

/**
 * @name Alignment Utilities
 * @{
 */

/**
 * @brief Checks if a value is a power of two and not zero.
 *
 * @param value Value to check.
 * @return true if value is a non-zero power of two, false otherwise.
 */
bool memory_is_power_of_two(uintptr_t value);

/**
 * @brief Returns the offset of a value within the given alignment boundary.
 *
 * For example, for alignment = 8 and value = 14, returns 6.
 *
 * @param value Value to check.
 * @param alignment Alignment boundary (must be a power of two).
 * @return Offset of value within alignment.
 */
uintptr_t memory_align_offset(uintptr_t value, uintptr_t alignment);

/**
 * @brief Checks if a value is aligned to the given alignment boundary.
 *
 * @param value Value to check.
 * @param alignment Alignment boundary (must be a power of two).
 * @return true if value is aligned, false otherwise.
 */
bool memory_is_aligned(uintptr_t value, uintptr_t alignment);

/**
 * @brief Aligns a value up to the next multiple of the given alignment.
 *
 * If already aligned, returns value unchanged.
 *
 * @param value Value to align.
 * @param alignment Alignment boundary (must be a power of two).
 * @return Aligned value rounded up.
 */
uintptr_t memory_align_up(uintptr_t value, uintptr_t alignment);

/**
 * @brief Aligns a value down to the previous multiple of the given alignment.
 *
 * If already aligned, returns value unchanged.
 *
 * @param value Value to align.
 * @param alignment Alignment boundary (must be a power of two).
 * @return Aligned value rounded down.
 */
uintptr_t memory_align_down(uintptr_t value, uintptr_t alignment);

/**
 * @brief Aligns a byte size up to the nearest system page size.
 *
 * @param value Byte size to align.
 * @return Byte size aligned up to page size.
 */
size_t memory_align_up_pagesize(size_t value);

/**
 * @brief Returns the number of padding bytes needed to align an address up to alignment.
 *
 * Returns zero if the address is already aligned.
 *
 * @param value Address to check.
 * @param alignment Alignment boundary (must be a power of two).
 * @return Number of padding bytes needed.
 */
size_t memory_padding_needed(uintptr_t value, size_t alignment);

/**
 * @brief Returns the minimal count of objects of size 'object_size' required
 *        to cover 'value' bytes, after rounding 'value' up to the given alignment.
 *
 * Useful to compute how many aligned units are needed to contain a buffer.
 *
 * @param value Required size in bytes.
 * @param size Size of each object in bytes.
 * @param alignment Alignment boundary (must be a power of two).
 * @return Minimal number of objects required.
 */
uintptr_t memory_align_unit_count(uintptr_t value, uintptr_t size, uintptr_t alignment);

/** @} */

/**
 * @name Aligned Memory Allocation
 * @{
 */

/**
 * @brief Allocates memory of given size aligned to alignment boundary.
 *
 * Uses posix_memalign internally. The returned pointer must be freed with free().
 *
 * @param size Number of bytes to allocate.
 * @param alignment Alignment boundary; must be a power of two and >= sizeof(void *).
 * @return Pointer to allocated memory on success, or NULL on failure.
 */
void* memory_alloc(size_t size, size_t alignment);

/**
 * @brief Allocates zero-initialized memory for an array with specified alignment.
 *
 * Equivalent to calloc but guarantees alignment.
 *
 * @param n Number of elements.
 * @param size Size of each element.
 * @param alignment Alignment boundary; must be a power of two.
 * @return Pointer to zeroed memory on success, or NULL on failure.
 */
void* memory_calloc(size_t n, size_t size, size_t alignment);

/**
 * @brief Reallocates aligned memory block to new size with alignment guarantee.
 *
 * If ptr is NULL, behaves like memory_alloc.
 * If new_size is zero, frees ptr and returns NULL.
 *
 * The original ptr must have been allocated with memory_alloc or memory_calloc.
 *
 * @param ptr Pointer to the memory block to reallocate.
 * @param old_size Size of the original allocation in bytes.
 * @param new_size New size in bytes.
 * @param alignment Desired alignment boundary (must be a power of two).
 * @return Pointer to newly allocated memory, or NULL on failure.
 */
void* memory_realloc(void* ptr, size_t old_size, size_t new_size, size_t alignment);

/**
 * @brief Frees memory allocated by memory_alloc or memory_calloc.
 *
 * @param ptr Pointer to memory block to free.
 */
void memory_free(void* ptr);

/** @} */

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DSA_MEMORY_H
