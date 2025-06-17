/**
 * Copyright © 2023 Austin Berrio
 *
 * @file tests/container/test_list.c
 *
 * @note Keep fixtures and tests as simple as reasonably possible.
 *       - Tests must be kept simple. The simpler, the better.
 *       - Tests must return an integer with 0 on success and 1 on failure.
 *         - I'll admit this is weird, but the shell and cmake test for this.
 */

#include "core/memory.h"
#include "core/logger.h"
#include "container/list.h"
#include "test/unit.h"

typedef struct TestListData {
    ContainerList* list;
    int value;
    int expected;
} TestListData;

int test_unit_list_setup(TestUnit* unit) {
    TestListData* data = (TestListData*) unit->data;
    data->list = container_list_create();
    ASSERT(data->list, "Failed to create list.");
    return 0;
}

int test_unit_list_teardown(TestUnit* unit) {
    TestListData* data = (TestListData*) unit->data;
    container_list_free(data->list);
    return 0;
}

int test_group_list_append_and_get(TestUnit* unit) {
    TestListData* data = (TestListData*) unit->data;

    container_list_append(data->list, &data->value);
    void* result = container_list_get_data(data->list, 0);

    ASSERT(result == &data->value, "Expected %p, got %p", (void*) &data->value, result);
    return 0;
}

int test_suite_list(void) {
    TestListData data[] = {
        {.value = 5, .expected = 5},
        {.value = 3, .expected = 3},
        {.value = 7, .expected = 7},
    };

    size_t count = sizeof(data) / sizeof(TestListData);
    TestUnit units[count];
    for (size_t i = 0; i < (count); i++) {
        (units)[i].data = &(data)[i];
    }

    TestGroup group = {
        .name = "Container List",
        .count = count,
        .units = units,
        .run = test_group_list_append_and_get,
        .before_each = test_unit_list_setup,
        .after_each = test_unit_list_teardown,
    };

    return test_group_run(&group);
}

/**
 * @brief Main function to run all unit tests.
 *
 * @return 0 on success, non-zero on failure
 */
int main(void) {
    TestSuite suites[] = {
        {"Container List", test_suite_list},
    };

    int result = 0;
    size_t total = sizeof(suites) / sizeof(TestSuite);
    for (size_t i = 0; i < total; i++) {
        result |= test_suite_run(&suites[i]);
    }

    return result;
}
