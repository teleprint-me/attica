/**
 * @file include/allocator/page.h
 * @brief Page-based memory allocator with metadata tracking.
 *
 * This interface provides a simple allocator built on top of a linear-probing hash map.
 * Each allocation is tracked with size and alignment metadata, allowing for:
 * - Manual allocation and deallocation.
 * - Safe reallocation with metadata updates.
 * - Global deallocation of all tracked memory.
 *
 * Internally, the allocator stores metadata (`Page`) in the provided `HashMap` context.
 *
 * @note This API does not perform internal locking. The caller is responsible for synchronization.
 * @note All allocations and frees must use this API consistently to avoid memory leaks.
 */

#ifndef ALLOCATOR_PAGE_H
#define ALLOCATOR_PAGE_H

#include "map/linear.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Allocates a block of memory with the given size and alignment.
 *
 * A metadata entry is inserted into the provided `HashMap` context to track the allocation.
 *
 * @param ctx Pointer to a valid `HashMap` used for tracking.
 * @param size Number of bytes to allocate.
 * @param alignment Alignment constraint (must be power of two).
 * @return Pointer to the allocated memory, or NULL on failure.
 */
void* hash_page_malloc(HashMap* ctx, size_t size, size_t alignment);

/**
 * @brief Reallocates a previously allocated memory block to a new size and alignment.
 *
 * - If `ptr` is NULL, this acts like `hash_page_malloc`.
 * - If `size` is zero, this frees the memory block.
 * - On success, metadata is updated in-place and reassigned to the new address.
 *
 * @param ctx Pointer to a valid `HashMap` used for tracking.
 * @param ptr Pointer to the memory block to reallocate.
 * @param size New size in bytes.
 * @param alignment New alignment requirement.
 * @return Pointer to the reallocated memory, or NULL on failure.
 */
void* hash_page_realloc(HashMap* ctx, void* ptr, size_t size, size_t alignment);

/**
 * @brief Frees a memory block and removes its tracking metadata from the context.
 *
 * Logs an error if the pointer was not tracked by the context.
 *
 * @param ctx Pointer to a valid `HashMap` used for tracking.
 * @param ptr Pointer to the memory block to free.
 */
void hash_page_free(HashMap* ctx, void* ptr);

/**
 * @brief Frees all memory blocks tracked by the context and clears the table.
 *
 * This is useful for bulk cleanup or teardown of a scoped allocator.
 *
 * @param ctx Pointer to a valid `HashMap` used for tracking.
 * @warning This is not thread-safe (yet).
 */
void hash_page_free_all(HashMap* ctx);

#ifdef __cplusplus
}
#endif

#endif // ALLOCATOR_PAGE_H
