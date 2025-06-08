/// @file tests/allocator/test_knr.c

#include "core/memory.h"
#include "core/logger.h"
#include "test/unit.h"

#include "allocator/knr.h"

typedef struct TestKNRAllocator {
    void* address;
    size_t size;
    int expected; // 0: expect NULL, 1: expect success
} TestKNRAllocator;

int knr_alloc_setup(TestUnit* unit) {
    TestKNRAllocator* data = (TestKNRAllocator*) unit->data;
    data->address = allocator_freelist_malloc(data->size);
    return 0;
}

int knr_alloc_teardown(TestUnit* unit) {
    TestKNRAllocator* data = (TestKNRAllocator*) unit->data;
    if (data->address) {
        allocator_freelist_free(data->address);
        data->address = NULL;
    }
    return 0;
}

int knr_alloc_assert(TestUnit* unit) {
    TestKNRAllocator* data = (TestKNRAllocator*) unit->data;
    if (data->expected) {
        ASSERT(
            data->address,
            "[KNR] unit=%zu with size=%zu failed to allocate",
            unit->index,
            data->size
        );
        ASSERT(
            ((size_t) data->address % MEMORY_ALIGNMENT) == 0,
            "[KNR] Improperly aligned segment in unit=%zu with size=%zu, expected=0, got=%zu,",
            unit->index,
            data->size,
            ((size_t) data->address % MEMORY_ALIGNMENT)
        );
    } else {
        ASSERT(
            !data->address,
            "[KNR] unit=%zu with size=%zu expected to fail but succeeded",
            unit->index,
            data->size
        );
    }
    return 0;
}

int test_suite_knr_allocator(void) {
    // Find max allocatable size for this run
    size_t max_size = allocator_freelist_max_alloc();

    TestKNRAllocator data[] = {
        {NULL, 128, 1},
        {NULL, max_size, 1},
        {NULL, max_size + 1, 0}, // should fail
        {NULL, 0, 1}, // allocation of zero: depends on your allocator's policy
    };

    size_t count = sizeof(data) / sizeof(TestKNRAllocator);
    TestUnit units[count];
    for (size_t i = 0; i < count; i++) {
        units[i].data = &data[i];
    }

    TestGroup group = {
        .name = "KNR Allocator Isolated",
        .count = count,
        .units = units,
        .before_each = knr_alloc_setup,
        .run = knr_alloc_assert,
        .after_each = knr_alloc_teardown,
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
