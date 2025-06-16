/// @file src/allocator/handle.c
#include "allocator/handle.h"

void handle_allocator_init(HandleAllocator* a, size_t size, size_t alignment) {
    a->size = size;
    a->alignment = alignment;
    a->objects = memory_calloc(HANDLE_MAX_OBJECTS, size, alignment);
    a->freelist_count = HANDLE_MAX_OBJECTS;
    for (uint32_t i = 0; i < HANDLE_MAX_OBJECTS; ++i) {
        a->generations[i] = 1;
        a->freelist[i] = HANDLE_MAX_OBJECTS - 1 - i; // Fill backwards
    }
}

Handle handle_alloc(HandleAllocator* a) {
    if (a->freelist_count == 0) {
        return (Handle) {0xFFFFFFFF, 0};
    }

    uint32_t index = a->freelist[--a->freelist_count];
    return (Handle) {index, a->generations[index]};
}

bool handle_is_valid(const HandleAllocator* a, Handle h) {
    if (h.index >= HANDLE_MAX_OBJECTS) {
        return false;
    }
    return h.generation == a->generations[h.index];
}

void* handle_get(HandleAllocator* a, Handle h) {
    return handle_is_valid(a, h) ? &a->objects[h.index] : NULL;
}

void handle_free(HandleAllocator* a, Handle h) {
    if (!handle_is_valid(a, h)) {
        return;
    }
    a->generations[h.index]++;
    a->freelist[a->freelist_count++] = h.index;
    memory_free(a->objects);
}
