/// @file tests/allocator/test_knr.c

#include "core/memory.h"
#include "core/logger.h"
#include "test/unit.h"

#include "allocator/knr.h"

typedef struct TestKNRAllocator {
    size_t size1;
    size_t size2;
    size_t size3;
    int expect_null;
} TestKNRAllocator;

int test_group_knr_allocator(TestUnit* unit) {
    TestKNRAllocator* data = (TestKNRAllocator*) unit->data;
    void* a = allocator_freelist_malloc(data->size1);
    ASSERT(a != NULL, "[KNR] Allocation of size1 (%zu) failed", data->size1);
    ASSERT(((uintptr_t)a % MEMORY_ALIGNMENT) == 0, "[KNR] Pointer a not properly aligned");

    void* b = allocator_freelist_malloc(data->size2);
    ASSERT(b != NULL, "[KNR] Allocation of size2 (%zu) failed", data->size2);

    void* c = allocator_freelist_malloc(data->size3);

    if (data->expect_null) {
        ASSERT(c == NULL, "[KNR] Third allocation should have failed but did not");
    } else {
        ASSERT(c != NULL, "[KNR] Third allocation failed unexpectedly");
    }

    allocator_freelist_free(a);
    allocator_freelist_free(b);
    if (c) {
        allocator_freelist_free(c);
    }

    return 0;
}

int test_suite_knr_allocator(void) {
    // Compute max safe allocation
    TestKNRAllocator data[] = {
        {128, 256, 384, 0},
        {HEAP_WORDS, 1, 1, 1}, // Should fail on second or third alloc
        {HEAP_WORDS/2, HEAP_WORDS/2, 1, 1}, // Should fit two, not three
    };

    size_t count = sizeof(data) / sizeof(TestKNRAllocator);
    TestUnit units[count];
    for (size_t i = 0; i < count; i++) {
        units[i].data = &data[i];
    }

    TestGroup group = {
        .name = "KNR Free List Allocator",
        .count = count,
        .units = units,
        .run = test_group_knr_allocator,
    };

    return test_group_run(&group);
}

int main(void) {
    TestSuite suites[] = {
        {"KNR Free List Allocator", test_suite_knr_allocator},
    };

    int result = 0;
    size_t count = sizeof(suites) / sizeof(TestSuite);
    for (size_t i = 0; i < count; i++) {
        result |= test_suite_run(&suites[i]);
    }
    return result;
}
