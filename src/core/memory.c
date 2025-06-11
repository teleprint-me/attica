/**
 * Copyright © 2023 Austin Berrio
 *
 * @file src/core/memory.c
 * @brief Utility functions for memory alignment, padding, and allocation.
 *
 * Provides helper functions to:
 * - Check power-of-two properties
 * - Determine alignment of addresses or sizes
 * - Calculate padding and aligned sizes
 * - Allocate aligned memory blocks with posix_memalign
 */

#include "core/memory.h"

#include <string.h>

bool memory_is_power_of_two(uintptr_t value) {
    return (0 != value) && (0 == (value & (value - 1)));
}

uintptr_t memory_alignment_offset(uintptr_t value, uintptr_t alignment) {
    assert(memory_is_power_of_two(alignment));
    return value & (alignment - 1);
}

bool memory_is_aligned(uintptr_t value, uintptr_t alignment) {
    assert(memory_is_power_of_two(alignment));
    return 0 == memory_alignment_offset(value, alignment);
}

uintptr_t memory_align_up(uintptr_t value, uintptr_t alignment) {
    assert(memory_is_power_of_two(alignment));
    return (value + alignment - 1) & ~(alignment - 1);
}

uintptr_t memory_align_down(uintptr_t value, uintptr_t alignment) {
    assert(memory_is_power_of_two(alignment));
    return value & ~(alignment - 1);
}

size_t memory_padding_needed(uintptr_t address, size_t alignment) {
    assert(memory_is_power_of_two(alignment));
    size_t offset = memory_alignment_offset(address, alignment);
    return (0 != offset) ? alignment - offset : 0;
}

uintptr_t memory_object_count(uintptr_t nbytes, uintptr_t nsize, uintptr_t alignment) {
    assert(nsize > 0);
    uintptr_t aligned_size = memory_align_up(nbytes, alignment);
    return (aligned_size + nsize - 1) / nsize;
}

void* memory_aligned_alloc(size_t size, size_t alignment) {
    if (alignment < sizeof(void*)) {
        alignment = sizeof(void*);
    }

    if (!memory_is_power_of_two(alignment)) {
        return NULL;
    }

    void* address = NULL;
    if (0 != posix_memalign(&address, alignment, size)) {
        return NULL;
    }

    return address;
}

void* memory_aligned_calloc(size_t n, size_t size, size_t alignment) {
    size_t total = n * size;
    void* address = memory_aligned_alloc(total, alignment);
    if (address && memset(address, 0, total)) {
        return address;
    }

    return NULL;
}

void* memory_aligned_realloc(void* ptr, size_t old_size, size_t new_size, size_t alignment) {
    if (NULL == ptr) {
        return memory_aligned_alloc(new_size, alignment);
    }

    if (0 == new_size) {
        free(ptr);
        return NULL;
    }

    if (!memory_is_power_of_two(alignment)) {
        return NULL;
    }

    void* new_ptr = memory_aligned_alloc(new_size, alignment);
    if (NULL == new_ptr) {
        return NULL;
    }

    // Copy only the smaller of the old or new sizes
    size_t min_size = old_size < new_size ? old_size : new_size;
    memcpy(new_ptr, ptr, min_size);
    free(ptr);
    return new_ptr;
}

void memory_aligned_free(void* ptr) {
    if (NULL != ptr) {
        free(ptr);
    }
}
