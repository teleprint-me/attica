/**
 * Copyright © 2023 Austin Berrio
 *
 * @file tests/test_utf8_byte.c
 */

#include "logger.h"
#include "unit_test.h"
#include "utf8/byte.h"
#include "utf8/raw.h"

// --- UTF-8 Test Raw Validation ---

typedef struct UTF8TestRawIsValid {
    const char* label;
    const char* string;
    const bool expected;
} UTF8TestRawIsValid;

int test_utf8_raw_is_valid(TestCase* test) {
    UTF8TestRawIsValid* unit = (UTF8TestRawIsValid*) test->unit;
    bool actual = utf8_raw_is_valid(unit->string);
    ASSERT(
        actual == unit->expected, // this fails when false == unit->expected
        "Test case %zu (input: '%0x') failed: expected %s, got %s",
        test->index,
        unit->string,
        unit->expected ? "true" : "false",
        actual ? "true" : "false"
    );
    return 0;
}

int test_utf8_raw_is_valid_suite(void) {
    static UTF8TestRawIsValid units[] = {
        {"ASCII A", (char*) "\x41", true},
        {"Greeting", (char*) "Hello, world!", true},
        {"Greek", (char*) "Γεια σου κόσμο!", true},
        {"Empty", (char*) "", true},
        {"Empty", (char*) "\0", true},
        {"Newline", (char*) "\n", true},
        {"Space", (char*) " ", true},
        {"Mixed", (char*) "\x41 \nこんにちは、世界！", true},
    };

    size_t total_tests = sizeof(units) / sizeof(UTF8TestRawIsValid);
    TestCase test_cases[total_tests];

    for (size_t i = 0; i < total_tests; i++) {
        test_cases[i].unit = &units[i];
    }

    TestContext context = {
        .total_tests = total_tests,
        .test_name = "UTF-8 Raw Validity",
        .test_cases = test_cases,
    };

    return test_unit_run(&context, test_utf8_raw_is_valid, NULL);
}

int main(void) {
    typedef struct UTF8TestSuite {
        const char* label;
        int (*suite)(void);
    } UTF8TestSuite;

    static UTF8TestSuite test_suites[] = {
        {"UTF-8 Raw Validity", test_utf8_raw_is_valid_suite},
    };

    size_t total_suites = sizeof(test_suites) / sizeof(UTF8TestSuite);

    int result = 0;

    for (size_t i = 0; i < total_suites; i++) {
        result |= test_suite_run(test_suites[i].label, test_suites[i].suite);
    }

    return result;
}
