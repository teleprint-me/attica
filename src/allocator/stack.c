/**
 * @file include/stack.c
 */

#include "memory.h"
#include "stack.h"

#include <stdlib.h>
#include <stdio.h>

Stack* stack_create(size_t capacity) {
    Stack* stack = memory_aligned_alloc(sizeof(Stack), alignof(Stack));
    if (!stack) {
        return NULL;
    }

    stack->buffer = memory_aligned_alloc(capacity, alignof(uint8_t));
    if (!stack->buffer) {
        return NULL;
    }

    stack->capacity = capacity;
    stack->offset = 0;
    stack->last_offset = NULL;
    stack->num_allocs = 0;
    stack->max_allocs = 0;
    return stack;
}

void* stack_push(Stack* stack, size_t size, size_t alignment) {
    assert(stack != NULL);
    assert(memory_is_power_of_two(alignment));

    uintptr_t current = (uintptr_t) (stack->buffer + stack->offset);
    size_t padding = memory_padding_needed(current, alignment);

    if (stack->offset + padding + size > stack->capacity) {
        return NULL; // Out of memory
    }

    // Grow last_offset array if needed
    if (stack->num_allocs >= stack->max_allocs) {
        stack->max_allocs = (stack->max_allocs == 0) ? 1 : stack->max_allocs * 2;
        stack->last_offset = realloc(stack->last_offset, stack->max_allocs * sizeof(uintptr_t));
        assert(stack->last_offset != NULL); // Ensure allocation success
    }

    // Store current offset before allocation
    stack->last_offset[stack->num_allocs++] = stack->offset;

    stack->offset += padding;
    void* pointer = stack->buffer + stack->offset;
    stack->offset += size;

    return pointer;
}

void stack_pop(Stack* stack) {
    assert(stack != NULL);

    if (stack->num_allocs == 0) {
        return; // Nothing to pop
    }

    // Restore previous offset
    stack->offset = stack->last_offset[--stack->num_allocs];

    // Shrink last_offset array if too large
    if (stack->num_allocs < stack->max_allocs / 4 && stack->max_allocs > 4) {
        stack->max_allocs /= 2;
        stack->last_offset = realloc(stack->last_offset, stack->max_allocs * sizeof(uintptr_t));
        assert(stack->last_offset != NULL);
    }
}

void* stack_peek(const Stack* stack) {
    if (stack->num_allocs == 0) {
        return NULL;
    }
    return stack->buffer + stack->last_offset[stack->num_allocs - 1];
}

bool stack_realloc(Stack* stack, size_t new_capacity, size_t alignment) {
    if (new_capacity <= stack->capacity) {
        return false; // Nothing to do
    }

    // Allocate a new, larger buffer
    void* new_buffer = memory_aligned_alloc(new_capacity, alignment);
    if (NULL == new_buffer) {
        return false;
    }

    // Copy existing contents
    for (size_t i = 0; i < stack->offset; ++i) {
        ((uint8_t*) new_buffer)[i] = stack->buffer[i];
    }

    // Free the old buffer
    free(stack->buffer);

    // Update stack metadata
    stack->buffer = new_buffer;
    stack->capacity = new_capacity;

    return true;
}

void stack_reset(Stack* stack) {
    if (stack) {
        if (stack->last_offset) {
            free(stack->last_offset);
            stack->last_offset = NULL;
        }
        stack->offset = 0;
        stack->capacity = 0;
        stack->num_allocs = 0;
        stack->max_allocs = 0;
    }
}

void stack_free(Stack* stack) {
    if (stack) {
        if (stack->buffer) {
            free(stack->buffer);
        }
        if (stack->last_offset) {
            free(stack->last_offset);
        }
        free(stack);
    }
}

size_t stack_used(const Stack* stack) {
    return stack->offset;
}

size_t stack_remaining(const Stack* stack) {
    return stack->capacity - stack->offset;
}

size_t stack_top(const Stack* stack) {
    return (stack->num_allocs > 0) ? stack->last_offset[stack->num_allocs - 1] : 0;
}

void stack_dump_buffer(const Stack* stack) {
    size_t used = stack->offset;
    printf("[Stack] Buffer Dump (%zu bytes used):\n", used);

    for (size_t i = 0; i < used; ++i) {
        if (i % 16 == 0) {
            printf("0x%04zx : ", i);
        }
        printf("%02x ", stack->buffer[i]);
        if ((i + 1) % 16 == 0 || i + 1 == used) {
            printf("\n");
        }
    }
}

void stack_dump_allocs(const Stack* stack) {
    uintptr_t used = stack->max_allocs;
    printf("[Stack] Allocs Dump (%zu bytes used):\n", used);

    for (size_t i = 0; i < used; ++i) {
        if (i % 16 == 0) {
            printf("0x%04zx : ", i);
        }
        printf("%02x ", (unsigned int) stack->last_offset[i]);
        if ((i + 1) % 16 == 0 || i + 1 == used) {
            printf("\n");
        }
    }
}

void stack_dump_info(const Stack* stack) {
    printf(
        "[Stack] 0x%p\n"
        "[Stack] offset: %zu / %zu (remaining: %zu)\n"
        "[Stack] last offset: 0x%p\n"
        "[Stack] allocs: %zu / %zu (remaining: %zu)\n",
        (const void*) stack,
        stack->offset,
        stack->capacity,
        stack->capacity - stack->offset,
        (const void*) stack->last_offset,
        stack->num_allocs,
        stack->max_allocs,
        stack->max_allocs - stack->num_allocs
    );
    for (size_t i = 0; i < stack->num_allocs; ++i) {
        printf("[Stack] alloc[%zu]: %zu\n", i, stack->last_offset[i]);
    }
}

void stack_dump(const Stack* stack) {
    stack_dump_allocs(stack);
    stack_dump_buffer(stack);
    stack_dump_info(stack);
}
