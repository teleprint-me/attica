/**
 * Copyright © 2023 Austin Berrio
 *
 * @file tests/utf8/test_utf8_raw.c
 */

#include "core/logger.h"
#include "test/unit.h"
#include "utf8/byte.h"
#include "utf8/raw.h"

// --- UTF-8 Test Raw Validation ---

typedef struct TestUTF8RawIsValid {
    const char* label;
    const char* string;
    const bool expected;
} TestUTF8RawIsValid;

int test_group_utf8_raw_is_valid(TestUnit* unit) {
    TestUTF8RawIsValid* data = (TestUTF8RawIsValid*) unit->data;

    bool actual = utf8_raw_is_valid(data->string);

    ASSERT(
        actual == data->expected, // this fails when false == data->expected
        "[TestUTF8RawIsValid] unit=%zu, input='%0x', expected='%s', got='%s'",
        unit->index,
        data->string,
        data->expected ? "true" : "false",
        actual ? "true" : "false"
    );

    return 0;
}

int test_suite_utf8_raw_is_valid(void) {
    static TestUTF8RawIsValid data[] = {
        {"ASCII A", (char*) "\x41", true},
        {"Greeting", (char*) "Hello, world!", true},
        {"Greek", (char*) "Γεια σου κόσμο!", true},
        {"Empty", (char*) "", true},
        {"Empty", (char*) "\0", true},
        {"Newline", (char*) "\n", true},
        {"Space", (char*) " ", true},
        {"Mixed", (char*) "\x41 \nこんにちは、世界！", true},
    };

    size_t count = sizeof(data) / sizeof(TestUTF8RawIsValid);
    TestUnit units[count];
    for (size_t i = 0; i < count; i++) {
        units[i].data = &data[i];
    }

    TestGroup group = {
        .name = "utf8_raw_is_valid",
        .count = count,
        .units = units,
        .run = test_group_utf8_raw_is_valid,
    };

    return test_group_run(&group);
}

int main(void) {
    TestSuite suites[] = {
        {"utf8_raw_is_valid", test_suite_utf8_raw_is_valid},
    };

    size_t count = sizeof(suites) / sizeof(TestSuite);
    int result = 0;
    for (size_t i = 0; i < count; i++) {
        result |= test_suite_run(&suites[i]);
    }
    return result;
}
