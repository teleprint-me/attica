/**
 * Copyright © 2023 Austin Berrio
 *
 * @file tests/core/memory.c
 * @brief
 *
 * Manual Build:
 *   gcc -o test_memory \
 *     -I./include -lpthread \
 *     src/core/logger.c src/core/memory.c src/test/unit.c tests/core/test_memory.c
 */

#include "core/logger.h"
#include "core/memory.h"
#include "test/unit.h"

typedef struct TestMemoryAlignmentOffset {
    uintptr_t value;
    uintptr_t alignment;
    uintptr_t expected; // output
} TestMemoryAlignmentOffset;

int test_group_memory_bitwise_offset(TestUnit* unit) {
    TestMemoryAlignmentOffset* data = (TestMemoryAlignmentOffset*) unit->data;

    uintptr_t result = memory_alignment_offset(data->value, data->alignment);

    ASSERT(
        result == data->expected,
        "[TestMemoryAlignmentOffset] index=%zu, expected=%p, got=%p",
        unit->index,
        data->expected,
        result
    );

    return 0;
}

int test_suite_memory_bitwise_offset(void) {
    TestMemoryAlignmentOffset data[] = {
        {0x00, 8, 0}, // aligned
        {0x01, 8, 1}, // 1 byte offset
        {0x07, 8, 7}, // just before alignment
        {0x08, 8, 0}, // exactly aligned
        {0x0F, 8, 7}, // offset again
        {0x10, 8, 0}, // aligned
        {0x11, 8, 1}, // unit wrap
        {0x1003, 8, 3}, // sample from your inline assert
        {0x1234, 16, 4}, // not aligned to 16
        {0x1234, 64, 52}, // edge case
    };

    size_t count = sizeof(data) / sizeof(TestMemoryAlignmentOffset);
    TestUnit units[count];
    for (size_t i = 0; i < count; i++) {
        units[i].data = &data[i];
    }

    TestGroup group = {
        .name = "memory_bitwise_offset",
        .count = count,
        .units = units,
        .run = test_group_memory_bitwise_offset,
    };

    return test_group_run(&group);
}

typedef struct TestMemoryIsPowerOfTwo {
    uintptr_t x;
    bool expected;
} TestMemoryIsPowerOfTwo;

int test_group_memory_is_power_of_two(TestUnit* unit) {
    TestMemoryIsPowerOfTwo* data = (TestMemoryIsPowerOfTwo*) unit->data;

    bool result = memory_is_power_of_two(data->x);

    ASSERT(
        result == data->expected,
        "[TestMemoryIsPowerOfTwo] index=%zu, input=%zu, expected=%d, got=%d",
        unit->index,
        data->x,
        data->expected,
        result
    );

    return 0;
}

int test_suite_memory_is_power_of_two(void) {
    TestMemoryIsPowerOfTwo data[] = {
        {0, false}, // edge case: zero
        {1, true}, // 2^0
        {2, true}, // 2^1
        {3, false}, // in-between
        {4, true}, // 2^2
        {7, false}, // just before 2^3
        {8, true}, // 2^3
        {15, false}, // just before 2^4
        {16, true}, // 2^4
        {1024, true}, // big
        {1023, false}, // just before big
        {UINTPTR_MAX, false}, // very large non-power-of-two
    };

    size_t count = sizeof(data) / sizeof(TestMemoryIsPowerOfTwo);
    TestUnit units[count];
    for (size_t i = 0; i < count; i++) {
        units[i].data = &data[i];
    }

    TestGroup group = {
        .name = "memory_is_power_of_two",
        .count = count,
        .units = units,
        .run = test_group_memory_is_power_of_two,
    };

    return test_group_run(&group);
}

typedef struct TestMemoryIsAligned {
    uintptr_t x;
    uintptr_t alignment;
    bool expected;
} TestMemoryIsAligned;

int test_group_memory_is_aligned(TestUnit* unit) {
    TestMemoryIsAligned* data = (TestMemoryIsAligned*) unit->data;

    bool result = memory_is_aligned(data->x, data->alignment);

    ASSERT(
        result == data->expected,
        "[TestMemoryIsAligned] index=%zu, input=%p align=%zu expected=%d got=%d",
        unit->index,
        (void*)data->x,
        data->alignment,
        data->expected,
        result
    );

    return 0;
}

int test_suite_memory_is_aligned(void) {
    TestMemoryIsAligned data[] = {
        {0x00, 8, true},
        {0x08, 8, true},
        {0x09, 8, false},
        {0x10, 8, true},
        {0x11, 8, false},
        {0x20, 16, true},
        {0x23, 16, false},
        {0x40, 64, true},
        {0x41, 64, false},
        {0xFF, 1, true}, // all addresses aligned to 1
        {0x1000, 4096, true},
        {0x1001, 4096, false},
    };

    size_t count = sizeof(data) / sizeof(TestMemoryIsAligned);
    TestUnit units[count];
    for (size_t i = 0; i < count; i++) {
        units[i].data = &data[i];
    }

    TestGroup group = {
        .name = "memory_is_aligned",
        .count = count,
        .units = units,
        .run = test_group_memory_is_aligned,
    };

    return test_group_run(&group);
}

int main(void) {
    TestSuite suites[] = {
        {"memory_bitwise_offset", test_suite_memory_bitwise_offset},
        {"memory_is_power_of_two", test_suite_memory_is_power_of_two},
        {"memory_is_aligned", test_suite_memory_is_aligned},
    };

    int result = 0;
    size_t count = sizeof(suites) / sizeof(TestSuite);
    for (size_t i = 0; i < count; i++) {
        result |= test_suite_run(&suites[i]);
    }
    return result;
}
