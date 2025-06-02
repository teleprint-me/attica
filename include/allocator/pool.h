/**
 * Copyright © 2023 Austin Berrio
 *
 * @file include/allocator/pool.h
 */

#ifndef ALLOCATOR_POOL_H
#define ALLOCATOR_POOL_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct FreeList {
    struct FreeList* next;
} FreeList;

typedef struct Pool {
    uint8_t* buffer;
    size_t capacity;
    size_t block_size;
    size_t block_count;
    FreeList* head;
} Pool;

Pool* pool_create(size_t capacity, size_t size, size_t alignment);
bool pool_realloc(Pool* pool, size_t new_capacity);
void pool_free(Pool* pool);

void* pool_push(Pool* pool);
void pool_pop(Pool* pool, void* address);

size_t pool_used(const Pool* pool); // # of used blocks
size_t pool_remaining(const Pool* pool); // # of free blocks
bool pool_owns(const Pool* pool, const void* address); // Is ptr within pool buffer range?

void pool_dump_info(Pool* pool);
void pool_dump_buffer(Pool* pool, size_t bytes);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // ALLOCATOR_POOL_H
