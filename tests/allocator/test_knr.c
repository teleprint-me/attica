/// @file tests/allocator/test_knr.c
#include "allocator/knr.h"

int main(void) {
    void* a = allocator_freelist_malloc(128);
    void* b = allocator_freelist_malloc(256);
    allocator_freelist_free(a);
    allocator_freelist_free(b);
    allocator_freelist_dump();

    void* c = allocator_freelist_malloc(384); // Should re-use/free space
    allocator_freelist_dump();

    allocator_freelist_free(c);
    allocator_freelist_dump();
}
