/// @file tests/allocator/test_knr.c

#include "core/memory.h"
#include "core/logger.h"
#include "test/unit.h"

#include "allocator/knr.h"

#define ONE_MB (1024 * 1024)
#define TEN_MB (10 * ONE_MB)
#define HUNDRED_MB (100 * ONE_MB)

typedef enum TestKNRAllocatorState {
    KNR_EXPECT_ALLOCATED, // Expect address
    KNR_EXPECT_UNALLOCATED, // Expect NULL
    KNR_EXPECT_UNDEFINED // Record result (policy-defined)
} TestKNRAllocatorState;

typedef struct TestKNRAllocator {
    void* address;
    size_t size;
    TestKNRAllocatorState state;
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
    switch (data->state) {
        case KNR_EXPECT_ALLOCATED:
            ASSERT(
                data->address,
                "[KNR] unit=%zu with size=%zu: expected allocation, got NULL",
                unit->index,
                data->size
            );
            ASSERT(
                ((size_t) data->address % MEMORY_ALIGNMENT) == 0,
                "[KNR] unit=%zu with size=%zu: allocation not aligned",
                unit->index,
                data->size
            );
            break;
        case KNR_EXPECT_UNALLOCATED:
            ASSERT(
                !data->address,
                "[KNR] unit=%zu with size=%zu: expected NULL, got allocation",
                unit->index,
                data->size
            );
            break;
        case KNR_EXPECT_UNDEFINED:
        default:
            LOG_WARNING(
                "[KNR] unit=%zu with size=%zu: undefined behavior, got %p",
                unit->index,
                data->size,
                data->address
            );
            break;
    }
    return 0;
}

int test_suite_knr_allocator(void) {
    TestKNRAllocator data[] = {
        {NULL, 128, KNR_EXPECT_ALLOCATED},
        {NULL, ONE_MB, KNR_EXPECT_ALLOCATED},
        {NULL, TEN_MB, KNR_EXPECT_ALLOCATED},
        {NULL, HUNDRED_MB, KNR_EXPECT_ALLOCATED},
        {NULL, (size_t) -1, KNR_EXPECT_UNALLOCATED}, // absurd size (policy: returns NULL)
        {NULL, 0, KNR_EXPECT_UNALLOCATED}, // allocation of zero (policy: returns NULL)
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
