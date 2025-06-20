/**
 * @file src/allocator/page.c
 */

#include "core/memory.h"
#include "core/logger.h"
#include "allocator/page.h"

/**
 * @section Private
 * {@
 */

/// @brief Opaque object storing metadata as a value in a given PageTable
typedef struct PageEntry {
    size_t size;
    size_t alignment;
} PageEntry;

PageEntry* page_entry_create(size_t size, size_t alignment) {
    PageEntry* page = memory_calloc(1, sizeof(PageEntry), alignof(PageEntry));
    if (NULL == page) {
        return NULL;
    }

    *page = (PageEntry) {
        .size = size,
        .alignment = alignment,
    };

    return page;
}

void page_entry_free(PageEntry* page) {
    if (NULL == page) {
        return;
    }
    memory_free(page);
}

/** @} */

/**
 * @section Page Allocation API
 * @{
 */

void* page_malloc(PageAllocator* allocator, size_t size, size_t alignment) {
    if (NULL == allocator) {
        LOG_ERROR("[PA_MALLOC] Missing allocation context (PageAllocator)");
        return NULL;
    }

    void* address = memory_alloc(size, alignment);
    if (NULL == address) {
        LOG_ERROR("[PA_MALLOC] Allocation failed (size=%zu, align=%zu)", size, alignment);
        return NULL;
    }

    PageEntry* page = page_entry_create(size, alignment);
    if (NULL == page) {
        memory_free(address);
        LOG_ERROR("[PA_MALLOC] Failed to allocate page metadata for %p", address);
        return NULL;
    }

    HashMapState state = hash_map_insert(allocator, address, page);
    if (HASH_MAP_STATE_FULL == state) {
        // Attempt to resize
        state = hash_map_resize(allocator, allocator->size * 2);
        if (HASH_MAP_STATE_SUCCESS != state) {
            memory_free(address);
            page_entry_free(page);
            LOG_ERROR("[PA_MALLOC] Failed to resize page allocator.");
            return NULL;
        }

        // Retry insertion
        state = hash_map_insert(allocator, address, page);
    }

    if (HASH_MAP_STATE_SUCCESS != state) {
        memory_free(address);
        page_entry_free(page);
        LOG_ERROR(
            "[PA_MALLOC] Failed to insert %p into page allocator (state = %d)", address, state
        );
        return NULL;
    }

#if defined(DEBUG) && (1 == DEBUG)
    LOG_DEBUG("[PA_MALLOC] %p → %p (%zu bytes, %zu aligned)", address, page, size, alignment);
#endif

    return address;
}

void* page_realloc(PageAllocator* allocator, void* ptr, size_t size, size_t alignment) {
    // Null context
    if (NULL == allocator) {
        LOG_ERROR("[PA_REALLOC] Missing allocation context (page allocator)");
        return NULL;
    }

    // Fresh allocation
    if (NULL == ptr) {
        return page_malloc(allocator, size, alignment);
    }

    // Lookup existing metadata
    PageEntry* page = hash_map_search(allocator, ptr);
    if (NULL == page) {
        LOG_ERROR("[PA_REALLOC] Unknown pointer %p", ptr);
        return NULL;
    }

    // Vulkan signals free via realloc with size == 0
    if (0 == size) {
        if (HASH_MAP_STATE_SUCCESS != hash_map_delete(allocator, ptr)) {
            LOG_ERROR("[PA_REALLOC] Failed to remove page for %p", ptr);
        }
        page_entry_free(page);
        memory_free(ptr);
        return NULL;
    }

    void* address = memory_realloc(ptr, page->size, size, alignment);
    if (NULL == address) {
        LOG_ERROR("[PA_REALLOC] Failed to realloc %p (%zu → %zu bytes)", ptr, page->size, size);
        return NULL;
    }

    // Update page metadata in-place
    *page = (PageEntry) {
        .size = size,
        .alignment = alignment,
    };

    // Re-allocator page metadata to new address
    if (HASH_MAP_STATE_SUCCESS != hash_map_delete(allocator, ptr)) {
        LOG_ERROR("[PA_REALLOC] Failed to remove old mapping for %p", ptr);
        return NULL;
    }

    HashMapState state = hash_map_insert(allocator, address, page);
    if (HASH_MAP_STATE_FULL == state) {
        // Attempt to resize
        state = hash_map_resize(allocator, allocator->size * 2);
        if (HASH_MAP_STATE_SUCCESS != state) {
            memory_free(address);
            page_entry_free(page);
            LOG_ERROR("[PA_REALLOC] Failed to resize page allocator.");
            return NULL;
        }

        // Retry insertion
        state = hash_map_insert(allocator, address, page);
    }

    if (HASH_MAP_STATE_SUCCESS != state) {
        memory_free(address);
        page_entry_free(page);
        LOG_ERROR(
            "[PA_REALLOC] Failed to insert %p into page allocator (state = %d)", address, state
        );
        return NULL;
    }

#if defined(DEBUG) && (1 == DEBUG)
    LOG_DEBUG("[PA_REALLOC] %p → %p (%zu bytes)", ptr, address, size);
#endif

    return address;
}

void page_free(PageAllocator* allocator, void* ptr) {
    if (NULL == allocator || NULL == ptr) {
        return;
    }

    PageEntry* page = (PageEntry*) hash_map_search(allocator, ptr);
    if (NULL == page) {
        LOG_ERROR("[PA_FREE] Attempted to free untracked memory %p", ptr);
        return;
    }

    if (HASH_MAP_STATE_SUCCESS != hash_map_delete(allocator, ptr)) {
        LOG_ERROR("[PA_FREE] Failed to remove page for %p", ptr);
        return;
    }

#if defined(DEBUG) && (1 == DEBUG)
    LOG_DEBUG("[PA_FREE] %p (%zu bytes, %zu aligned)", ptr, page->size, page->alignment);
#endif

    page_entry_free(page);
    memory_free(ptr);
}

void page_free_all(PageAllocator* allocator) {
    if (NULL == allocator) {
        return;
    }

    HashMapIterator it = hash_map_iter(allocator);
    HashMapEntry* entry = NULL;

    while ((entry = hash_map_next(&it))) {
        void* ptr = entry->key;
        PageEntry* page = (PageEntry*) entry->value;

        if (ptr && page) {
#if defined(DEBUG) && (1 == DEBUG)
            LOG_DEBUG(
                "[PA_CLEAR] Freeing %p (%zu bytes, %zu aligned)", ptr, page->size, page->alignment
            );
#endif
            page_entry_free(page);
            memory_free(ptr);
        }
    }

    hash_map_clear(allocator); // Clears internal map state
}

/** @} */

/**
 * @section Allocator Lifecycle
 * @{
 */

PageAllocator* page_allocator_create(size_t initial_size) {
    return hash_map_create(initial_size, HASH_MAP_KEY_TYPE_ADDRESS);
}

void page_allocator_free(PageAllocator* allocator) {
    if (NULL == allocator) {
        return;
    }
    page_free_all(allocator);
    hash_map_free(allocator);
}

/** @} */

/**
 * @section Debug Utilities
 * @{
 */

void page_allocator_dump(PageAllocator* allocator) {
    if (NULL == allocator) {
        return;
    }

    size_t total = 0;
    HashMapIterator it = hash_map_iter(allocator);
    HashMapEntry* entry = NULL;

    while ((entry = hash_map_next(&it))) {
        PageEntry* page = (PageEntry*) entry->value;
        void* ptr = entry->key;

        if (ptr && page) {
            total += page->size;
            LOG_INFO("[PA_DUMP] %p (%zu bytes, %zu aligned)", ptr, page->size, page->alignment);
        }
    }

    LOG_INFO("[PA_DUMP] Total memory still tracked: %zu bytes", total);
}

/** @} */
