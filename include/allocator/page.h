/**
 * @file include/allocator/page.h
 */

#ifndef ALLOCATOR_PAGE_H
#define ALLOCATOR_PAGE_H

#include "map/linear.h"
#include <stddef.h>

void* hash_page_malloc(HashMap* ctx, size_t size, size_t alignment);
void* hash_page_realloc(HashMap* ctx, void* ptr, size_t size, size_t alignment);
void hash_page_free(HashMap* ctx, void* ptr);

#endif // ALLOCATOR_PAGE_H
