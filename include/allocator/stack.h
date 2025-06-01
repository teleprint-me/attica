/**
 * Copyright © 2023 Austin Berrio
 *
 * @file include/allocator/stack.h
 */

#ifndef ALLOCATOR_STACK_H
#define ALLOCATOR_STACK_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct {
    uint8_t* buffer; // Memory buffer
    size_t capacity; // Total memory size
    uintptr_t offset; // Current offset
    uintptr_t* last_offset; // Dynamic array of previous offsets
    size_t num_allocs; // Number of allocations
    size_t max_allocs; // Capacity of last_offset array
} Stack;

Stack* stack_create(size_t capacity);
void stack_free(Stack* stack);

void* stack_push(Stack* stack, size_t size, size_t alignment);
void stack_pop(Stack* stack); // Rolls back to previous allocation
void* stack_peek(const Stack* stack);

bool stack_realloc(Stack* stack, size_t new_capacity, size_t alignment);
void stack_reset(Stack* stack); // Resets entire stack

size_t stack_used(const Stack* stack);
size_t stack_remaining(const Stack* stack);
size_t stack_top(const Stack* stack);

void stack_dump_buffer(const Stack* stack);
void stack_dump_allocs(const Stack* stack);
void stack_dump_info(const Stack* stack);
void stack_dump(const Stack* stack);

#endif // ALLOCATOR_STACK_H
