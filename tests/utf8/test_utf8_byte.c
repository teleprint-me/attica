/**
 * Copyright © 2023 Austin Berrio
 *
 * @file tests/utf8/test_utf8_byte.c
 */

#include "core/logger.h"
#include "test/unit.h"
#include "utf8/byte.h"

/**
 * UTF8 Byte Width
 */

typedef struct TestUTF8ByteWidth {
    const char* label;
    const uint8_t* bytes;
    const int8_t expected;
} TestUTF8ByteWidth;

int test_group_utf8_byte_width(TestUnit* unit) {
    TestUTF8ByteWidth* data = (TestUTF8ByteWidth*) unit->data;
    int8_t actual = utf8_byte_width(data->bytes);

    // Check if the actual length is greater than 0
    ASSERT(
        actual > 0,
        "[TestUTF8ByteWidth] Invalid lead byte: unit=%zu, data='%s'",
        unit->index,
        data->bytes
    );

    // Check if the actual length matches the expected length
    ASSERT(
        actual == data->expected,
        "[TestUTF8ByteWidth] Invalid byte length: unit=%zu, data='%s', expected=%d, got=%d",
        unit->index,
        data->bytes,
        data->expected,
        actual
    );

    return 0; // Success
}

int test_suite_utf8_byte_width(void) {
    TestUTF8ByteWidth data[] = {
        {"Empty", (const uint8_t*) "", 1},
        {"ASCII NULL", (const uint8_t*) "\0", 1},
        {"ASCII a", (const uint8_t*) "a", 1},
        {"ASCII DEL", (const uint8_t*) "\x7F", 1},
        {"2-byte ¢", (const uint8_t*) "\u00A2", 2},
        {"3-byte €", (const uint8_t*) "\u20AC", 3},
        {"4-byte 😀", (const uint8_t*) "\U0001F600", 4},
    };

    size_t count = sizeof(data) / sizeof(TestUTF8ByteWidth);
    TestUnit units[count];
    for (size_t i = 0; i < count; i++) {
        units[i].data = &data[i];
    }

    TestGroup group = {
        .name = "utf8_byte_width",
        .count = count,
        .units = units,
        .run = test_group_utf8_byte_width,
    };

    return test_group_run(&group);
}

/**
 * UTF8 Byte Is Valid
 */

typedef struct TestUTF8ByteIsValid {
    const char* label;
    const uint8_t* bytes;
    const bool expected;
} TestUTF8ByteIsValid;

int test_group_utf8_byte_is_valid(TestUnit* unit) {
    TestUTF8ByteIsValid* data = (TestUTF8ByteIsValid*) unit->data;

    int8_t width = utf8_byte_width(data->bytes);
    if (-1 == width) {
        ASSERT(
            false == data->expected, // if this is set to false, this is silent
            "[TestUTF8ByteIsValid] Expected invalid sequence but got valid: unit=%zu, input='%0x'",
            unit->index,
            *data->bytes
        );
    }

    bool actual = utf8_byte_is_valid(data->bytes);
    ASSERT(
        actual == data->expected, // this fails when false == data->expected
        "[TestUTF8ByteIsValid] unit=%zu, input='%0x', expected='%s', got='%s'",
        unit->index,
        *data->bytes,
        data->expected ? "true" : "false",
        actual ? "true" : "false"
    );

    return 0;
}

int test_suite_utf8_byte_is_valid(void) {
    TestUTF8ByteIsValid data[] = {
        {"Valid Empty", (uint8_t*) "", true},
        {"Valid NULL", (uint8_t*) "\x00", true},
        {"Valid /", (uint8_t*) "/", true},
        {"ASCII A", (uint8_t*) "\x41", true},
        {"Continuation 0x80", (uint8_t*) "\x80", false},
        {"Continuation 0xBF", (uint8_t*) "\xBF", false},
        {"Invalid Start 0xC0", (uint8_t*) "\xC0", false},
        {"Invalid Start 0xC1", (uint8_t*) "\xC1", false},
        {"Invalid 5-byte start", (uint8_t*) "\xF8", false},
        {"Overlong A (U+0041)", (uint8_t*) "\xE0\x81\x81", false},
        {"Overlong /", (uint8_t*) "\xC0\xAF", false},
        {"Overlong encoding", (uint8_t*) "\xE0\x80", false},
        {"Truncated 2-byte (start only)", (uint8_t*) "\xC2", false},
        {"Truncated 3-byte (2 of 3)", (uint8_t*) "\xE2\x82", false},
        {"Truncated 4-byte (3 of 4)", (uint8_t*) "\xF0\x9F\x92", false},
        {"3-byte with bad continuation", (uint8_t*) "\xE2\x28\xAC", false},
        {"4-byte with bad continuation (early)", (uint8_t*) "\xF0\x28\x8C\xBC", false},
        {"4-byte with bad continuation (late)", (uint8_t*) "\xF0\x9F\x98\x28", false},
        {"Application Program Command (159)", (uint8_t*) "\xC2\x9F", true},
        {"2-byte ¢", (uint8_t*) "\xC2\xA2", true},
        {"3-byte €", (uint8_t*) "\xE2\x82\xAC", true},
        {"4-byte 😀", (uint8_t*) "\xF0\x9F\x98\x80", true},
        {"Surrogate D800", (uint8_t*) "\xED\xA0\x80", false},
    };

    size_t count = sizeof(data) / sizeof(TestUTF8ByteIsValid);
    TestUnit units[count];
    for (size_t i = 0; i < count; i++) {
        units[i].data = &data[i];
    }

    TestGroup group = {
        .name = "utf8_byte_is_valid",
        .count = count,
        .units = units,
        .run = test_group_utf8_byte_is_valid,
    };

    return test_group_run(&group);
}

/**
 * UTF8 Byte Is Equal
 */

typedef struct TestUTF8ByteIsEqual {
    const char* label;
    const uint8_t* a;
    const uint8_t* b;
    bool expected;
} TestUTF8ByteIsEqual;

int test_group_utf8_byte_is_equal(TestUnit* unit) {
    TestUTF8ByteIsEqual* data = (TestUTF8ByteIsEqual*) unit->data;
    bool result = utf8_byte_is_equal(data->a, data->b);

    ASSERT(
        result == data->expected,
        "[TestUTF8ByteIsEqual] [%s] expected='%s', got='%s'",
        data->label,
        data->expected ? "true" : "false",
        result ? "true" : "false"
    );

    return 0;
}

int test_suite_utf8_byte_is_equal(void) {
    TestUTF8ByteIsEqual data[] = {
        {"Equal ASCII", (uint8_t*) "A", (uint8_t*) "A", true},
        {"Unequal ASCII", (uint8_t*) "A", (uint8_t*) "B", false},
        {"Equal 2-byte (¢)", (uint8_t*) "\xC2\xA2", (uint8_t*) "\xC2\xA2", true},
        {"Equal 3-byte (€)", (uint8_t*) "\xE2\x82\xAC", (uint8_t*) "\xE2\x82\xAC", true},
        {"Different 3-byte", (uint8_t*) "\xE2\x82\xAC", (uint8_t*) "\xE2\x80\xA2", false},
        {"Equal (¢ literal vs encoded)", (uint8_t*) "\xC2\xA2", (uint8_t*) "¢", true},
        {"Fullwidth ≠ Standard Cent", (uint8_t*) "\xC2\xA2", (uint8_t*) "\xEF\xBF\xA0", false},
        {"Invalid A", (uint8_t*) "\xC0", (uint8_t*) "\xC2\xA2", false},
        {"Invalid B", (uint8_t*) "\xC2\xA2", (uint8_t*) "\xC1", false},
        {"Both invalid", (uint8_t*) "\xED\xA0\x80", (uint8_t*) "\xC0", false},
    };

    size_t count = sizeof(data) / sizeof(TestUTF8ByteIsEqual);
    TestUnit units[count];
    for (size_t i = 0; i < count; i++) {
        units[i].data = &data[i];
    }

    TestGroup group = {
        .name = "utf8_byte_is_equal",
        .count = count,
        .units = units,
        .run = test_group_utf8_byte_is_equal,
    };

    return test_group_run(&group);
}

/**
 * UTF8 Byte Range
 */

typedef struct TestUTF8ByteRange {
    const char* label;
    const uint8_t* start;
    const uint8_t* end;
    ptrdiff_t expected;
} TestUTF8ByteRange;

int test_group_utf8_byte_range(TestUnit* unit) {
    TestUTF8ByteRange* data = (TestUTF8ByteRange*) unit->data;
    ptrdiff_t actual = utf8_byte_range(data->start, data->end);

    ASSERT(
        actual == data->expected,
        "[TestUTF8ByteRange] [%s] expected=%td, got=%td",
        data->label,
        data->expected,
        actual
    );

    return 0;
}

int test_suite_utf8_byte_range(void) {
    // 'h','e','l','l','o' (5 bytes), ¢ (2 bytes), € (3 bytes)
    static const uint8_t bytes[] = "hello¢€";

    TestUTF8ByteRange data[] = {
        {"ASCII h → o", &bytes[0], &bytes[5], 5},
        {"¢ span", &bytes[5], &bytes[7], 2},
        {"€ span", &bytes[7], &bytes[10], 3},
        {"Empty span", &bytes[2], &bytes[2], 0},
        {"NULL start", NULL, &bytes[1], -1},
        {"NULL end", &bytes[0], NULL, -1},
    };

    size_t count = sizeof(data) / sizeof(TestUTF8ByteRange);
    TestUnit units[count];
    for (size_t i = 0; i < count; i++) {
        units[i].data = &data[i];
    }

    TestGroup group = {
        .name = "utf8_byte_range",
        .count = count,
        .units = units,
        .run = test_group_utf8_byte_range,
    };

    return test_group_run(&group);
}

/**
 * UTF-8 Test Iterator
 */

/// @todo

int main(void) {
    TestSuite suites[] = {
        {"utf8_byte_width", test_suite_utf8_byte_width},
        {"utf8_byte_is_valid", test_suite_utf8_byte_is_valid},
        {"utf8_byte_is_equal", test_suite_utf8_byte_is_equal},
        {"utf8_byte_range", test_suite_utf8_byte_range},
    };

    size_t count = sizeof(suites) / sizeof(TestSuite);
    int result = 0;
    for (size_t i = 0; i < count; i++) {
        result |= test_suite_run(&suites[i]);
    }
    return result;
}
