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

typedef struct Page {
    size_t size;
    size_t alignment;
} Page;

Page* page_create(size_t size, size_t alignment) {
    Page* page = memory_alloc(sizeof(Page), alignof(Page));
    if (NULL == page) {
        return NULL;
    }

    *page = (Page) {
        .size = size,
        .alignment = alignment,
    };

    return page;
}

void page_free(Page* page) {
    if (NULL == page) {
        return;
    }
    memory_free(page);
}

/** @} */

/**
 * @section Public
 * {@
 */

void* hash_page_malloc(HashMap* ctx, size_t size, size_t alignment) {
    if (NULL == ctx) {
        LOG_ERROR("[HP_MALLOC] Missing allocation context (HashMap)");
        return NULL;
    }

    void* address = memory_alloc(size, alignment);
    if (NULL == address) {
        LOG_ERROR("[HP_MALLOC] Allocation failed (size=%zu, align=%zu)", size, alignment);
        return NULL;
    }

    Page* page = page_create(size, alignment);
    if (NULL == page) {
        memory_free(address);
        LOG_ERROR("[HP_MALLOC] Failed to allocate page metadata for %p", address);
        return NULL;
    }

    HashMapState state = hash_map_insert(ctx, address, page);
    if (HASH_MAP_STATE_FULL == state) {
        // Attempt to resize
        state = hash_map_resize(ctx, ctx->size * 2);
        if (state != HASH_MAP_STATE_SUCCESS) {
            memory_free(address);
            page_free(page);
            LOG_ERROR("[HP_MALLOC] Failed to resize page ctx.");
            return NULL;
        }

        // Retry insertion
        state = hash_map_insert(ctx, address, page);
    }

    if (HASH_MAP_STATE_SUCCESS != state) {
        memory_free(address);
        page_free(page);
        LOG_ERROR("[HP_MALLOC] Failed to insert %p into page ctx (state = %d)", address, state);
        return NULL;
    }

#if defined(DEBUG) && (1 == DEBUG)
    LOG_DEBUG("[HP_MALLOC] %p → %p (%zu bytes, %zu aligned)", address, page, size, alignment);
#endif

    return address;
}

void* hash_page_realloc(HashMap* ctx, void* ptr, size_t size, size_t alignment) {
    // Null context
    if (NULL == ctx) {
        LOG_ERROR("[HP_REALLOC] Missing allocation context (page ctx)");
        return NULL;
    }

    // Fresh allocation
    if (NULL == ptr) {
        return hash_page_malloc(ctx, size, alignment);
    }

    // Lookup existing metadata
    Page* page = hash_map_search(ctx, ptr);
    if (NULL == page) {
        LOG_ERROR("[HP_REALLOC] Unknown pointer %p", ptr);
        return NULL;
    }

    // Vulkan signals free via realloc with size == 0
    if (0 == size) {
        if (HASH_MAP_STATE_SUCCESS != hash_map_delete(ctx, ptr)) {
            LOG_ERROR("[HP_REALLOC] Failed to remove page for %p", ptr);
        }
        page_free(page);
        memory_free(ptr);
        return NULL;
    }

    void* address = memory_realloc(ptr, page->size, size, alignment);
    if (NULL == address) {
        LOG_ERROR("[HP_REALLOC] Failed to realloc %p (%zu → %zu bytes)", ptr, page->size, size);
        return NULL;
    }

    // Update page metadata in-place
    *page = (Page) {
        .size = size,
        .alignment = alignment,
    };

    // Re-ctx page metadata to new address
    if (HASH_MAP_STATE_SUCCESS != hash_map_delete(ctx, ptr)) {
        LOG_ERROR("[HP_REALLOC] Failed to remove old mapping for %p", ptr);
        return NULL;
    }

    HashMapState state = hash_map_insert(ctx, address, page);
    if (HASH_MAP_STATE_FULL == state) {
        // Attempt to resize
        state = hash_map_resize(ctx, ctx->size * 2);
        if (state != HASH_MAP_STATE_SUCCESS) {
            memory_free(address);
            page_free(page);
            LOG_ERROR("[HP_MALLOC] Failed to resize page ctx.");
            return NULL;
        }

        // Retry insertion
        state = hash_map_insert(ctx, address, page);
    }

    if (HASH_MAP_STATE_SUCCESS != state) {
        LOG_ERROR("[HP_REALLOC] Failed to insert new mapping for %p", address);
        return NULL;
    }

#if defined(DEBUG) && (1 == DEBUG)
    LOG_DEBUG("[HP_REALLOC] %p → %p (%zu bytes)", ptr, address, size);
#endif

    return address;
}

void hash_page_free(HashMap* ctx, void* ptr) {
    if (NULL == ctx || NULL == ptr) {
        return;
    }

    Page* page = (Page*) hash_map_search(ctx, ptr);
    if (NULL == page) {
        LOG_ERROR("[HP_FREE] Attempted to free untracked memory %p", ptr);
        return;
    }

    if (HASH_MAP_STATE_SUCCESS != hash_map_delete(ctx, ptr)) {
        LOG_ERROR("[HP_FREE] Failed to remove page for %p", ptr);
        return;
    }

#if defined(DEBUG) && (1 == DEBUG)
    LOG_DEBUG("[HP_FREE] %p (%zu bytes, %zu aligned)", ptr, page->size, page->alignment);
#endif

    page_free(page);
    memory_free(ptr);
}

void hash_page_free_all(HashMap* ctx) {
    if (!ctx) {
        return;
    }

    HashMapIterator it = hash_map_iter(ctx);
    HashMapEntry* entry = NULL;

    while ((entry = hash_map_next(&it))) {
        void* ptr = entry->key;
        Page* page = (Page*) entry->value;

        if (ptr && page) {
#if defined(DEBUG) && (1 == DEBUG)
            LOG_DEBUG(
                "[HP_CLEAR] Freeing %p (%zu bytes, %zu aligned)", ptr, page->size, page->alignment
            );
#endif
            page_free(page);
            memory_free(ptr);
        }
    }

    hash_map_clear(ctx); // Clears internal map state
}

void hash_page_dump(HashMap* ctx) {
    if (!ctx) {
        return;
    }

    size_t total = 0;
    HashMapIterator it = hash_map_iter(ctx);
    HashMapEntry* entry = NULL;

    while ((entry = hash_map_next(&it))) {
        Page* page = (Page*) entry->value;
        void* ptr = entry->key;

        if (ptr && page) {
            total += page->size;
            LOG_INFO("[HP_DUMP] %p (%zu bytes, %zu aligned)", ptr, page->size, page->alignment);
        }
    }

    LOG_INFO("[HP_DUMP] Total memory still tracked: %zu bytes", total);
}

/** @} */
