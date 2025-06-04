/**
 * Copyright © 2023 Austin Berrio
 *
 * @file tests/test_utf8_byte.c
 */

#include "core/logger.h"
#include "test/unit.h"
#include "utf8/byte.h"

// --- UTF-8 Test Byte Width ---

typedef struct UTF8TestByteWidth {
    const char* label;
    const uint8_t* bytes;
    const int8_t expected;
} UTF8TestByteWidth;

int test_utf8_byte_width(TestCase* test) {
    UTF8TestByteWidth* unit = (UTF8TestByteWidth*) test->unit;
    int8_t actual = utf8_byte_width(unit->bytes);

    // Check if the actual length is greater than 0
    ASSERT(
        actual > 0,
        "Invalid UTF-8 leading byte in test case %zu (unit: '%s')",
        test->index,
        unit->bytes
    );

    // Check if the actual length matches the expected length
    ASSERT(
        actual == unit->expected,
        "Invalid UTF-8 byte length in test case %zu (unit: '%s', expected: %d, got: %d)",
        test->index,
        unit->bytes,
        unit->expected,
        actual
    );

    return 0; // Success
}

int test_utf8_byte_width_suite(void) {
    static UTF8TestByteWidth units[] = {
        {"Empty", (const uint8_t*) "", 1},
        {"ASCII NULL", (const uint8_t*) "\0", 1},
        {"ASCII a", (const uint8_t*) "a", 1},
        {"ASCII DEL", (const uint8_t*) "\x7F", 1},
        {"2-byte ¢", (const uint8_t*) "\u00A2", 2},
        {"3-byte €", (const uint8_t*) "\u20AC", 3},
        {"4-byte 😀", (const uint8_t*) "\U0001F600", 4},
    };

    size_t total_tests = sizeof(units) / sizeof(UTF8TestByteWidth);
    TestCase test_cases[total_tests];

    for (size_t i = 0; i < total_tests; i++) {
        test_cases[i].unit = &units[i];
    }

    TestContext context = {
        .total_tests = total_tests,
        .test_name = "UTF-8 Byte Width",
        .test_cases = test_cases,
    };

    return test_unit_run(&context, test_utf8_byte_width, NULL);
}

// --- UTF-8 Test Byte Validation ---

typedef struct UTF8TestByteIsValid {
    const char* label;
    const uint8_t* bytes;
    const bool expected;
} UTF8TestByteIsValid;

int test_utf8_byte_is_valid(TestCase* test) {
    UTF8TestByteIsValid* unit = (UTF8TestByteIsValid*) test->unit;

    int8_t width = utf8_byte_width(unit->bytes);
    if (-1 == width) {
        ASSERT(
            false == unit->expected, // if this is set to false, this is silent
            "Expected invalid sequence but got valid: test case %zu (input: '%0x')",
            test->index,
            *unit->bytes
        );
    }

    bool actual = utf8_byte_is_valid(unit->bytes);
    ASSERT(
        actual == unit->expected, // this fails when false == unit->expected
        "Test case %zu (input: '%0x') failed: expected %s, got %s",
        test->index,
        *unit->bytes,
        unit->expected ? "true" : "false",
        actual ? "true" : "false"
    );

    return 0;
}

int test_utf8_byte_is_valid_suite(void) {
    static UTF8TestByteIsValid units[] = {
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

    size_t total_tests = sizeof(units) / sizeof(UTF8TestByteIsValid);
    TestCase test_cases[total_tests];

    for (size_t i = 0; i < total_tests; i++) {
        test_cases[i].unit = &units[i];
    }

    TestContext context = {
        .total_tests = total_tests,
        .test_name = "UTF-8 Byte Validity",
        .test_cases = test_cases,
    };

    return test_unit_run(&context, test_utf8_byte_is_valid, NULL);
}

// --- UTF-8 Test Byte Equality ---

typedef struct UTF8TestByteIsEqual {
    const char* label;
    const uint8_t* a;
    const uint8_t* b;
    bool expected;
} UTF8TestByteIsEqual;

int test_utf8_byte_is_equal(TestCase* test) {
    UTF8TestByteIsEqual* unit = (UTF8TestByteIsEqual*) test->unit;
    bool result = utf8_byte_is_equal(unit->a, unit->b);

    ASSERT(
        result == unit->expected,
        "[%s] expected %s, got %s",
        unit->label,
        unit->expected ? "true" : "false",
        result ? "true" : "false"
    );

    return 0;
}

int test_utf8_byte_is_equal_suite(void) {
    static UTF8TestByteIsEqual units[] = {
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

    size_t total_tests = sizeof(units) / sizeof(UTF8TestByteIsEqual);
    TestCase test_cases[total_tests];

    for (size_t i = 0; i < total_tests; i++) {
        test_cases[i].unit = &units[i];
    }

    TestContext context = {
        .total_tests = total_tests,
        .test_name = "UTF-8 Byte Equality",
        .test_cases = test_cases,
    };

    return test_unit_run(&context, test_utf8_byte_is_equal, NULL);
}

// --- UTF-8 Test Byte Range ---

typedef struct UTF8TestByteRange {
    const char* label;
    const uint8_t* start;
    const uint8_t* end;
    ptrdiff_t expected;
} UTF8TestByteRange;

int test_utf8_byte_range(TestCase* test) {
    UTF8TestByteRange* unit = (UTF8TestByteRange*) test->unit;
    ptrdiff_t actual = utf8_byte_range(unit->start, unit->end);

    ASSERT(
        actual == unit->expected, "[%s] expected %td, got %td", unit->label, unit->expected, actual
    );

    return 0;
}

int test_utf8_byte_range_suite(void) {
    // 'h','e','l','l','o' (5 bytes), ¢ (2 bytes), € (3 bytes)
    static const uint8_t bytes[] = "hello¢€";
    static UTF8TestByteRange units[] = {
        {"ASCII h → o", &bytes[0], &bytes[5], 5},
        {"¢ span", &bytes[5], &bytes[7], 2},
        {"€ span", &bytes[7], &bytes[10], 3},
        {"Empty span", &bytes[2], &bytes[2], 0},
        {"NULL start", NULL, &bytes[1], -1},
        {"NULL end", &bytes[0], NULL, -1},
    };

    size_t total_tests = sizeof(units) / sizeof(UTF8TestByteRange);
    TestCase test_cases[total_tests];

    for (size_t i = 0; i < total_tests; i++) {
        test_cases[i].unit = &units[i];
    }

    TestContext context = {
        .total_tests = total_tests,
        .test_name = "UTF-8 Byte Range",
        .test_cases = test_cases,
    };

    return test_unit_run(&context, test_utf8_byte_range, NULL);
}

// --- UTF-8 Test Iterator ---

/// @todo

int main(void) {
    typedef struct UTF8TestSuite {
        const char* label;
        int (*suite)(void);
    } UTF8TestSuite;

    static UTF8TestSuite test_suites[] = {
        {"UTF-8 Byte Width", test_utf8_byte_width_suite},
        {"UTF-8 Byte Validity", test_utf8_byte_is_valid_suite},
        {"UTF-8 Byte Equality", test_utf8_byte_is_equal_suite},
        {"UTF-8 Byte Range", test_utf8_byte_range_suite},
    };

    size_t total_suites = sizeof(test_suites) / sizeof(UTF8TestSuite);

    int result = 0;

    for (size_t i = 0; i < total_suites; i++) {
        result |= test_suite_run(test_suites[i].label, test_suites[i].suite);
    }

    return result;
}
