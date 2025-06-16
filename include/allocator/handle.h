/// @file include/allocator/handle.h
#ifndef DSA_ALLOCATOR_HANDLE_H
#define DSA_ALLOCATOR_HANDLE_H

#include "core/memory.h"

#define HANDLE_MAX_OBJECTS 1024

typedef struct {
    uint32_t index;
    uint32_t generation;
} Handle;

typedef struct {
    void* objects;
    size_t size;
    size_t alignment;
    uint32_t freelist_count;
    uint32_t freelist[HANDLE_MAX_OBJECTS];
    uint32_t generations[HANDLE_MAX_OBJECTS];
} HandleAllocator;

void allocator_handle_init(HandleAllocator* a, size_t size, size_t alignment);
Handle allocator_handle_create(HandleAllocator* a);
void allocator_handle_free(HandleAllocator* a, Handle h);

bool allocator_handle_is_valid(const HandleAllocator* a, Handle h);
void* allocator_handle_get(HandleAllocator* a, Handle h);

#endif // DSA_ALLOCATOR_HANDLE_H
