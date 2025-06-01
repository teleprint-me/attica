/**
 * Copyright © 2023 Austin Berrio
 *
 * @file src/allocator/arena.c
 * @brief Source file for the Linear Arena memory allocator.
 *
 * This module provides a simple linear memory arena implementation that allows
 * efficient memory allocation and deallocation without frequent calls to malloc
 * and free. The arena allocates memory in large chunks, and allocations are done
 * sequentially, which makes deallocation faster and easier by simply resetting
 * the arena. The arena supports reallocating and checkpoints for memory management.
 */

#include "allocator/memory.h"
#include "allocator/arena.h"

#include <stdlib.h>
#include <malloc.h>
#include <stdalign.h>

// Public methods
Arena* arena_create(size_t capacity) {
    // Allocate memory to the arena
    Arena* arena = memory_aligned_alloc(sizeof(Arena), alignof(Arena));
    if (!arena) {
        return NULL;
    }

    // Allocate memory to the buffer
    arena->buffer = memory_aligned_alloc(capacity, alignof(uint8_t));
    if (!arena->buffer) {
        free(arena);
        return NULL;
    }

    // set default attributes
    arena->capacity = capacity;
    arena->offset = 0;
    arena->last_offset = 0;
    return arena;
}

void* arena_alloc(Arena* arena, size_t size, size_t alignment) {
    uintptr_t current = (uintptr_t) (arena->buffer + arena->offset);
    size_t padding = memory_padding_needed(current, alignment);

    if (arena->offset + padding + size > arena->capacity) {
        return NULL;
    }

    arena->offset += padding;
    void* pointer = arena->buffer + arena->offset;
    arena->offset += size;

    return pointer;
}

bool arena_realloc(Arena* arena, size_t new_capacity, size_t alignment) {
    if (new_capacity <= arena->capacity) {
        return false; // Nothing to do
    }

    // Allocate a new, larger buffer
    void* new_buffer = memory_aligned_alloc(new_capacity, alignment);
    if (NULL == new_buffer) {
        return false;
    }

    // Copy existing contents
    for (size_t i = 0; i < arena->offset; ++i) {
        ((uint8_t*) new_buffer)[i] = arena->buffer[i];
    }

    // Free the old buffer
    free(arena->buffer);

    // Update arena metadata
    arena->buffer = new_buffer;
    arena->capacity = new_capacity;

    return true;
}

void arena_reset(Arena* arena) {
    if (arena) {
        arena->offset = 0;
        arena->last_offset = 0;
    }
}

void arena_free(Arena* arena) {
    if (arena) {
        if (arena->buffer) {
            free(arena->buffer);
        }
        free(arena);
    }
}

// Optional utilities
ArenaCheckpoint arena_checkpoint_begin(Arena* arena) {
    return (ArenaCheckpoint){
        .arena = arena,
        .offset = arena->offset,
        .last_offset = arena->last_offset
    };
}

void arena_checkpoint_end(ArenaCheckpoint checkpoint) {
    checkpoint.arena->offset = checkpoint.offset;
    checkpoint.arena->last_offset = checkpoint.last_offset;
}

// Optional introspection/debug
size_t arena_used(const Arena* arena) {
    return arena->offset;
}

size_t arena_remaining(const Arena* arena) {
    return arena->capacity - arena->offset;
}

void arena_debug(const Arena* arena) {
    printf(
        "[Arena] offset: %zu / %zu (remaining: %zu)\n",
        arena->offset,
        arena->capacity,
        arena_remaining(arena)
    );
}
