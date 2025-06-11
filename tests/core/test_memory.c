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

/**
 * @name Memory is Power of Two
 */

typedef struct TestMemoryIsPowerOfTwo {
    uintptr_t value;
    bool expected;
} TestMemoryIsPowerOfTwo;

int test_group_memory_is_power_of_two(TestUnit* unit) {
    TestMemoryIsPowerOfTwo* data = (TestMemoryIsPowerOfTwo*) unit->data;

    bool result = memory_is_power_of_two(data->value);

    ASSERT(
        result == data->expected,
        "[TestMemoryIsPowerOfTwo] index=%zu, input=%zu, expected=%d, got=%d",
        unit->index,
        data->value,
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

/** @} */

/**
 * @name Memory Align Offset
 */

typedef struct TestMemoryAlignOffset {
    uintptr_t value;
    uintptr_t alignment;
    uintptr_t expected; // output
} TestMemoryAlignOffset;

int test_group_memory_bitwise_offset(TestUnit* unit) {
    TestMemoryAlignOffset* data = (TestMemoryAlignOffset*) unit->data;

    uintptr_t result = memory_align_offset(data->value, data->alignment);

    ASSERT(
        result == data->expected,
        "[TestMemoryAlignOffset] index=%zu, expected=%p, got=%p",
        unit->index,
        data->expected,
        result
    );

    return 0;
}

int test_suite_memory_bitwise_offset(void) {
    TestMemoryAlignOffset data[] = {
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

    size_t count = sizeof(data) / sizeof(TestMemoryAlignOffset);
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

/** @} */

/**
 * @name Memory is Aligned
 */

typedef struct TestMemoryIsAligned {
    uintptr_t value;
    uintptr_t alignment;
    bool expected;
} TestMemoryIsAligned;

int test_group_memory_is_aligned(TestUnit* unit) {
    TestMemoryIsAligned* data = (TestMemoryIsAligned*) unit->data;

    bool result = memory_is_aligned(data->value, data->alignment);

    ASSERT(
        result == data->expected,
        "[TestMemoryIsAligned] index=%zu, input=%p align=%zu expected=%d got=%d",
        unit->index,
        (void*) data->value,
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

/** @} */

/**
 * @name Memory Align Up/Down
 */

typedef struct TestMemoryAlign {
    uintptr_t value;
    uintptr_t alignment;
    uintptr_t expected;
} TestMemoryAlign;

/** @} */

/**
 * @name Memory Align Up
 */

int test_group_memory_align_up(TestUnit* unit) {
    TestMemoryAlign* data = (TestMemoryAlign*) unit->data;

    uintptr_t result = memory_align_up(data->value, data->alignment);

    ASSERT(
        result == data->expected,
        "[TestMemoryAlignUp] index=%zu, value=%p, alignment=%zu, expected=%p, got=%p",
        unit->index,
        (void*) data->value,
        data->alignment,
        (void*) data->expected,
        (void*) result
    );

    return 0;
}

int test_suite_memory_align_up(void) {
    TestMemoryAlign data[] = {
        {0x00, 8, 0x00}, // already aligned
        {0x01, 8, 0x08}, // next aligned
        {0x07, 8, 0x08}, // boundary
        {0x08, 8, 0x08}, // exact
        {0x09, 8, 0x10}, // next block
        {0x10, 8, 0x10}, // aligned
        {0x11, 8, 0x18}, // wrap
        {0x1234, 16, 0x1240}, // test 16 alignment
        {0x1234, 64, 0x1240}, // test 64 alignment
        {UINTPTR_MAX - 7, 8, UINTPTR_MAX - 7}, // safe, already aligned
        {UINTPTR_MAX - 6, 8, UINTPTR_MAX}, // triggers overflow guard
    };

    size_t count = sizeof(data) / sizeof(TestMemoryAlign);
    TestUnit units[count];
    for (size_t i = 0; i < count; ++i) {
        units[i].index = i;
        units[i].data = &data[i];
    }

    TestGroup group = {
        .name = "memory_align_up",
        .count = count,
        .units = units,
        .run = test_group_memory_align_up,
    };

    return test_group_run(&group);
}

/** @} */

/**
 * @name Memory Align Down
 */

int test_group_memory_align_down(TestUnit* unit) {
    TestMemoryAlign* data = (TestMemoryAlign*) unit->data;

    uintptr_t result = memory_align_down(data->value, data->alignment);

    ASSERT(
        result == data->expected,
        "[TestMemoryAlignDown] index=%zu, value=%p, alignment=%zu, expected=%p, got=%p",
        unit->index,
        (void*) data->value,
        data->alignment,
        (void*) data->expected,
        (void*) result
    );

    return 0;
}

int test_suite_memory_align_down(void) {
    TestMemoryAlign data[] = {
        {0x00, 8, 0x00}, // already aligned
        {0x01, 8, 0x00}, // before boundary
        {0x07, 8, 0x00}, // just before boundary
        {0x08, 8, 0x08}, // exactly aligned
        {0x09, 8, 0x08}, // after aligned
        {0x1234, 16, 0x1230},
        {0x1234, 64, 0x1200},
        {UINTPTR_MAX, 8, UINTPTR_MAX & ~(uintptr_t) (8 - 1)},
    };

    size_t count = sizeof(data) / sizeof(TestMemoryAlign);
    TestUnit units[count];
    for (size_t i = 0; i < count; ++i) {
        units[i].index = i;
        units[i].data = &data[i];
    }

    TestGroup group = {
        .name = "memory_align_down",
        .count = count,
        .units = units,
        .run = test_group_memory_align_down,
    };

    return test_group_run(&group);
}

/** @} */

/**
 * @name Memory Align up Page Size
 */

/// @todo

/** @} */

/**
 * @name Memory Padding Needed
 */

typedef struct TestMemoryPadding {
    uintptr_t value;
    size_t alignment;
    size_t expected;
} TestMemoryPadding;

int test_group_memory_padding_needed(TestUnit* unit) {
    TestMemoryPadding* data = (TestMemoryPadding*) unit->data;

    size_t result = memory_padding_needed(data->value, data->alignment);

    ASSERT(
        result == data->expected,
        "[TestMemoryPadding] index=%zu, value=%p, alignment=%zu, expected=%zu, got=%zu",
        unit->index,
        (void*) data->value,
        data->alignment,
        data->expected,
        result
    );

    return 0;
}

int test_suite_memory_padding_needed(void) {
    TestMemoryPadding data[] = {
        {0x00, 8, 0},
        {0x01, 8, 7},
        {0x07, 8, 1},
        {0x08, 8, 0},
        {0x09, 8, 7},
        {0x1234, 16, 0x10 - (0x1234 % 0x10)}, // 0x1234 % 16 = 4 → padding = 12
        {0x1234, 64, 64 - (0x1234 % 64)}, // 0x1234 % 64 = 52 → padding = 12
        {0x1234, 128, 128 - (0x1234 % 128)}, // 0x1234 % 128 = 52 → padding = 76
        {0x1234, 1, 0}, // aligned to 1
    };

    size_t count = sizeof(data) / sizeof(TestMemoryPadding);
    TestUnit units[count];
    for (size_t i = 0; i < count; ++i) {
        units[i].index = i;
        units[i].data = &data[i];
    }

    TestGroup group = {
        .name = "memory_padding_needed",
        .count = count,
        .units = units,
        .run = test_group_memory_padding_needed,
    };

    return test_group_run(&group);
}

int main(void) {
    TestSuite suites[] = {
        {"memory_bitwise_offset", test_suite_memory_bitwise_offset},
        {"memory_is_power_of_two", test_suite_memory_is_power_of_two},
        {"memory_is_aligned", test_suite_memory_is_aligned},
        {"memory_align_up", test_suite_memory_align_up},
        {"memory_align_down", test_suite_memory_align_down},
        {"memory_padding_needed", test_suite_memory_padding_needed},
    };

    int result = 0;
    size_t count = sizeof(suites) / sizeof(TestSuite);
    for (size_t i = 0; i < count; i++) {
        result |= test_suite_run(&suites[i]);
    }
    return result;
}
