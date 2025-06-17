/**
 * Copyright © 2023 Austin Berrio
 *
 * @file tests/container/test_list.c
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

int test_unit_list_append_and_get_data(TestUnit* unit) {
    TestListData* data = (TestListData*) unit->data;

    container_list_append(data->list, &data->value);
    void* result = container_list_get_data(data->list, 0);

    ASSERT(result == &data->value, "Expected %p, got %p", (void*) &data->value, result);
    return 0;
}

int test_suite_list_append_and_get_data(void) {
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
        .name = "Append and Get Data",
        .count = count,
        .units = units,
        .run = test_unit_list_append_and_get_data,
        .before_each = test_unit_list_setup,
        .after_each = test_unit_list_teardown,
    };

    return test_group_run(&group);
}

typedef struct TestListIndex {
    ContainerList* list;
    size_t expected;
    int value;
} TestListIndex;

int test_group_list_setup(TestGroup* group) {
    ContainerList* list = container_list_create();
    TestUnit* units = (TestUnit*) group->units;
    for (size_t i = 0; i < group->count; i++) {
        TestListIndex* data = (TestListIndex*) (units)[i].data;
        data->list = list;
        container_list_append(list, &data->value);
    }
    group->shared = (const void*) list;
    return 0;
}

int test_group_list_teardown(TestGroup* group) {
    ContainerList* list = (ContainerList*) group->shared;
    container_list_free(list);
    return 0;
}

int test_unit_list_append_and_get_index(TestUnit* unit) {
    TestListIndex* data = (TestListIndex*) unit->data;
    size_t result = container_list_get_index(data->list, &data->value);
    ASSERT(result == data->expected, "Expected %zu, got %zu", data->expected, result);
    return 0;
}

int test_suite_list_append_and_get_index(void) {
    TestListIndex data[] = {
        {.value = 5, .expected = 0},
        {.value = 3, .expected = 1},
        {.value = 7, .expected = 2},
    };

    size_t count = sizeof(data) / sizeof(TestListIndex);
    TestUnit units[count];
    for (size_t i = 0; i < (count); i++) {
        (units)[i].data = &(data)[i];
    }

    TestGroup group = {
        .name = "Append and Get Index",
        .count = count,
        .units = units,
        .run = test_unit_list_append_and_get_index,
        .before_all = test_group_list_setup,
        .after_all = test_group_list_teardown,
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
        {"Append and Get Data", test_suite_list_append_and_get_data},
        {"Append and Get Index", test_suite_list_append_and_get_index},
    };

    int result = 0;
    size_t total = sizeof(suites) / sizeof(TestSuite);
    for (size_t i = 0; i < total; i++) {
        result |= test_suite_run(&suites[i]);
    }

    return result;
}
