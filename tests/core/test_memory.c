/**
 * Copyright © 2023 Austin Berrio
 *
 * @file tests/core/memory.c
 * @brief
 */

#include "core/logger.h"
#include "core/memory.h"
#include "test/unit.h"

typedef struct TestBitwiseOffset {
    uintptr_t x;
    uintptr_t y;
    uintptr_t expected; // output
} TestBitwiseOffset;

int test_group_bitwise_offset(TestUnit* unit) {
    TestBitwiseOffset* data = (TestBitwiseOffset*) unit->data;

    uintptr_t result = memory_bitwise_offset(data->x, data->y);

    ASSERT(
        result == data->expected,
        "[TestBitwiseOffset] expected=%p, got=%p",
        data->expected,
        result
    );

    return 0;
}

int test_suite_bitwise_offset(void) {
    TestBitwiseOffset data[] = {
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

    size_t count = sizeof(data) / sizeof(TestBitwiseOffset);
    TestUnit units[count];
    for (size_t i = 0; i < count; i++) {
        units[i].data = &data[i];
    }

    TestGroup group = {
        .name = "[TestGroup] Memory Bitwise Offset",
        .count = count,
        .units = units,
        .run = test_group_bitwise_offset,
    };

    return test_group_run(&group);
}

typedef struct TestPowerOfTwo {
    uintptr_t x;
    bool expected;
} TestPowerOfTwo;

int test_group_power_of_two(TestUnit* unit) {
    TestPowerOfTwo* data = (TestPowerOfTwo*) unit->data;

    bool result = memory_is_power_of_two(data->x);

    ASSERT(
        result == data->expected,
        "[TestPowerOfTwo] input=%zu expected=%d, got=%d",
        data->x,
        data->expected,
        result
    );

    return 0;
}

int test_suite_power_of_two(void) {
    TestPowerOfTwo data[] = {
        {8, true},
        {7, false},
    };

    size_t count = sizeof(data) / sizeof(TestPowerOfTwo);
    TestUnit units[count];
    for (size_t i = 0; i < count; i++) {
        units[i].data = &data[i];
    }

    TestGroup group = {
        .name = "[TestGroup] Memory Power of Two",
        .count = count,
        .units = units,
        .run = test_group_power_of_two,
    };

    return test_group_run(&group);
}

int main(void) {
    TestSuite suites[] = {
        {"[TestSuite] Memory Bitwise Offset", test_suite_bitwise_offset},
        {"[TestSuite] Memory Power of Two", test_suite_power_of_two},
    };

    int result = 0;
    size_t count = sizeof(suites) / sizeof(TestSuite);
    for (size_t i = 0; i < count; i++) {
        result |= test_suite_run(&suites[i]);
    }
    return result;
}
