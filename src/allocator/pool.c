/**
 * Copyright © 2023 Austin Berrio
 *
 * @file src/allocator/pool.c
 */

#include "allocator/memory.h"
#include "allocator/pool.h"

#include <stdio.h>

Pool* pool_create(size_t capacity, size_t size, size_t alignment) {
    // Initialize the pool
    Pool* pool = memory_aligned_alloc(sizeof(Pool), alignof(Pool));
    if (!pool) {
        return NULL;
    }

    // Initialize the buffer
    pool->capacity = capacity;
    pool->buffer = (uint8_t*) memory_aligned_alloc(pool->capacity, alignof(uint8_t));
    if (!pool->buffer) {
        free(pool);
        return NULL;
    }

    // Calculate the block size
    pool->block_size = memory_aligned_size(size, alignment);
    assert(pool->block_size > sizeof(FreeList) && "Block size is too small");
    assert(pool->block_size < pool->capacity && "Buffer capacity is smaller than the block size");
    assert(pool->block_size % alignment == 0 && "Block size is not properly aligned");

    // Calculate the block count
    pool->block_count = pool->capacity / pool->block_size;

    // Allocate addresses from the buffer
    pool->head = NULL;
    for (size_t block = 0; block < pool->block_count; block++) {
        void* address = (void*) (pool->buffer + block * pool->block_size);
        FreeList* node = (FreeList*) address;
        node->next = pool->head;
        pool->head = node;
    }

    return pool;
}

bool pool_realloc(Pool* pool, size_t new_capacity) {
    assert(pool != NULL && "Pool is NULL");
    assert(pool->buffer != NULL && "Buffer is NULL");
    if (new_capacity <= pool->capacity) {
        return false; // Nothing to do
    }

    // Allocate new buffer with desired alignment
    uint8_t* new_buffer = memory_aligned_alloc(new_capacity, alignof(uint8_t));
    if (!new_buffer) {
        return false;
    }

    // Copy existing contents
    for (size_t i = 0; i < pool->capacity; ++i) {
        ((uint8_t*) new_buffer)[i] = pool->buffer[i];
    }

    // Free the old buffer
    free(pool->buffer);

    // Calculate the block counts
    size_t old_block_count = pool->block_count;
    size_t new_block_count = new_capacity / pool->block_size;

    // Update the pools references
    pool->buffer = new_buffer;
    pool->capacity = new_capacity;
    pool->block_count = new_block_count;

    // Extend freelist from newly available blocks
    for (size_t block = old_block_count; block < new_block_count; block++) {
        void* address = (void*) (pool->buffer + block * pool->block_size);
        FreeList* node = (FreeList*) address;
        node->next = pool->head;
        pool->head = node;
    }

    return true;
}

void pool_free(Pool* pool) {
    if (pool) {
        if (pool->buffer) {
            free(pool->buffer);
        }
        free(pool);
    }
}

void* pool_push(Pool* pool) {
    assert(pool != NULL && "Pool is NULL");

    if (!pool->head) {
        return NULL;
    }

    // Get latest free node
    FreeList* node = pool->head;

    // Pop free node
    pool->head = pool->head->next;

    // Zero memory by default
    return (void*) node;
}

void pool_pop(Pool* pool, void* address) {
    assert(pool != NULL && "Pool is NULL");
    assert(address != NULL && "Address is NULL");
    assert(pool_owns(pool, address) && "Address is out of bounds");

    FreeList* node;

    // Push free node
    node = (FreeList*) address;
    node->next = pool->head;
    pool->head = node;
}

size_t pool_used(const Pool* pool) {
    return pool->block_count - pool_remaining(pool);
}

size_t pool_remaining(const Pool* pool) {
    size_t count = 0;
    FreeList* node = pool->head;
    while (node) {
        count++;
        node = node->next;
    }
    return count;
}

bool pool_owns(const Pool* pool, const void* address) {
    uintptr_t addr = (uintptr_t) address;
    uintptr_t start = (uintptr_t) pool->buffer;
    uintptr_t end = start + pool->capacity;

    return (addr >= start) && (addr < end) && ((addr - start) % pool->block_size == 0);
}

void pool_dump_info(Pool* pool) {
    printf("Pool Info:\n");
    printf("  Capacity   : %zu bytes\n", pool->capacity);
    printf("  Block Size : %zu bytes\n", pool->block_size);
    printf("  Blocks     : %zu\n", pool->block_count);

    printf("\nFree List:\n");
    size_t i = 0;
    for (FreeList* node = pool->head; node != NULL; node = node->next, i++) {
        printf("  [%zu] %p\n", i, (void*) node);
    }

    printf("  Total Free : %zu blocks\n\n", i);
}

void pool_dump_buffer(Pool* pool, size_t bytes) {
    if (bytes > pool->capacity) {
        bytes = pool->capacity;
    }

    printf("Buffer Hexdump (first %zu bytes):\n", bytes);
    for (size_t i = 0; i < bytes; i++) {
        printf("%02x ", pool->buffer[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
    if (bytes % 16 != 0) {
        printf("\n");
    }
    printf("\n");
}
