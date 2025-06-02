/**
 * Copyright © 2023 Austin Berrio
 *
 * @file tests/core/memory.c
 * @brief
 */

#include "core/logger.h"
#include "core/memory.h"

#include "test/unit.h"


typedef struct MemoryTestBitwiseOffset {
    uintptr_t x;
    uintptr_t y;
    uintptr_t expected; // output
} MemoryTestBitwiseOffset;

int test_memory_bitwise_offset(TestCase* test) {
    MemoryTestBitwiseOffset* unit = (MemoryTestBitwiseOffset*) test->unit;

    uintptr_t result = memory_bitwise_offset(unit->x, unit->y);

    ASSERT(
        result == unit->expected,
        "[MemoryTestBitwiseOffset] expected=%p, got=%p",
        unit->expected,
        result
    );

    return 0;
}

int test_memory_bitwise_offset_suite(void) {
    static MemoryTestBitwiseOffset cases[] = {

    };

    size_t total_tests = sizeof(cases) / sizeof(MemoryTestBitwiseOffset);

    TestCase test_cases[total_tests];
    for (size_t i = 0; i < total_tests; i++) {
        test_cases[i].unit = &cases[i];
    }

    TestContext context = {
        .total_tests = total_tests,
        .test_name = "Memory Test Bitwise Offest",
        .test_cases = test_cases,
    };

    return test_unit_run(&context, test_memory_bitwise_offset, NULL);
}

typedef struct MemoryTestPowerOfTwo {
    uintptr_t x;
    bool expected;
} MemoryTestPowerOfTwo;

void test_memory_alignment(void) {
    assert(memory_is_power_of_two(8));
    assert(!memory_is_power_of_two(7));
    // uintptr_t x = 0x1003;
    // assert(memory_align_up(x, 8) == 0x1008);
    assert(memory_bitwise_offset(0x1003, 8) == 3);
    // ...more edge cases
}

int main(void) {
    static TestRegister suites[] = {
        {"Memory Bitwise Offset", test_memory_bitwise_offset_suite},
    };

    int result = 0;
    size_t total = sizeof(suites) / sizeof(TestRegister);
    for (size_t i = 0; i < total; i++) {
        result |= test_suite_run(suites[i].name, suites[i].test_suite);
    }

    return result;
}
