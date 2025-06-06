/**
 * @file tests/container/test_stack.c
 */

#include "core/logger.h"
#include "test/unit.h"
#include "container/stack.h"

typedef struct TestContainerStack {
    int value;
    int expected;
    ContainerStack* stack;
} TestContainerStack;

/**
 * Test stack as individual units.
 */

int test_unit_stack_setup(TestUnit* unit) {
    TestContainerStack* d = (TestContainerStack*) unit->data;
    d->stack = container_stack_create();
    ASSERT(NULL != d->stack, "Failed to create stack");
    return 0;
}

int test_unit_stack_teardown(TestUnit* unit) {
    TestContainerStack* d = (TestContainerStack*) unit->data;
    ASSERT(d->stack, "Stack is not initialized");
    container_stack_free(d->stack);
    d->stack = NULL;
    return 0;
}

int test_group_each_container_stack(TestUnit* unit) {
    TestContainerStack* d = (TestContainerStack*) unit->data;

    // Push
    ASSERT(container_stack_push(d->stack, &(d->value)), "Failed to push value: %d", d->value);

    // Peek
    int* peeked = (int*) container_stack_peek(d->stack);
    ASSERT(
        peeked && *peeked == d->expected,
        "Peek mismatch: got %d, expected %d",
        peeked ? *peeked : -1,
        d->expected
    );

    // Pop
    int* popped = (int*) container_stack_pop(d->stack);
    ASSERT(
        popped && *popped == d->expected,
        "Pop mismatch: got %d, expected %d",
        popped ? *popped : -1,
        d->expected
    );

    // After pop, should be empty
    void* should_be_null = container_stack_peek(d->stack);
    ASSERT(should_be_null == NULL, "Stack not empty after pop");

    return 0;
}

int test_suite_each_container_stack(void) {
    TestContainerStack data[] = {
        {.value = 1, .expected = 1, .stack = NULL},
        {.value = 2, .expected = 2, .stack = NULL},
        {.value = -42, .expected = -42, .stack = NULL},
    };

    size_t count = sizeof(data) / sizeof(TestContainerStack);
    TestUnit units[count];
    for (size_t i = 0; i < count; i++) {
        units[i].data = &data[i];
    }

    TestGroup group = {
        .name = "Container Stack Each",
        .count = count,
        .units = units,
        .run = test_group_each_container_stack,
        .before_each = test_unit_stack_setup,
        .after_each = test_unit_stack_teardown,
    };

    return test_group_run(&group);
}

/**
 * Test stack as a shared group.
 *
 * Note: This group shares a single stack instance across all tests to
 * simulate real-world batch usage and validate LIFO sequence. Do not
 * assume tests are independent!
 */

int test_group_stack_setup(TestGroup* group) {
    // Allocate a single shared stack
    ContainerStack* stack = container_stack_create();
    ASSERT(stack != NULL, "Failed to create stack");

    // Attach it to each test unit's data for easy access
    for (size_t i = 0; i < group->count; i++) {
        TestContainerStack* d = (TestContainerStack*) group->units[i].data;
        d->stack = stack;
    }

    // Pre-populate with initial values
    for (size_t i = 0; i < group->count; i++) {
        TestContainerStack* d = (TestContainerStack*) group->units[i].data;
        container_stack_push(stack, &d->value);
    }
    return 0;
}

int test_group_stack_teardown(TestGroup* group) {
    // Only need to free once
    TestContainerStack* first = (TestContainerStack*) group->units[0].data;
    container_stack_free(first->stack);
    return 0;
}

int test_group_all_container_stack(TestUnit* unit) {
    TestContainerStack* d = (TestContainerStack*) unit->data;

    int* peeked = (int*) container_stack_peek(d->stack);
    ASSERT(
        peeked && *peeked == d->expected,
        "Peek mismatch: got %d, expected %d",
        peeked ? *peeked : -1,
        d->expected
    );

    int* popped = (int*) container_stack_pop(d->stack);
    ASSERT(
        popped && *popped == d->expected,
        "Pop mismatch: got %d, expected %d",
        popped ? *popped : -1,
        d->expected
    );

    return 0;
}

int test_suite_all_container_stack(void) {
    TestContainerStack data[] = {
        {.value = 1, .expected = 42},
        {.value = -2, .expected = -2},
        {.value = 42, .expected = 1},
    };

    size_t count = sizeof(data) / sizeof(TestContainerStack);
    TestUnit units[count];
    for (size_t i = 0; i < count; i++) {
        units[i].data = &data[i];
    }

    TestGroup group = {
        .name = "Container Stack All",
        .count = count,
        .units = units,
        .run = test_group_all_container_stack,
        .before_all = test_group_stack_setup,
        .after_all = test_group_stack_teardown,
    };

    return test_group_run(&group);
}

int main(void) {
    TestSuite suites[] = {
        {"Container Stack Each", test_suite_each_container_stack},
        {"Container Stack All", test_suite_all_container_stack}, // <-- FIXED NAME
    };

    int result = 0;
    size_t count = sizeof(suites) / sizeof(TestSuite);
    for (size_t i = 0; i < count; i++) {
        result |= test_suite_run(&suites[i]);
    }

    return result;
}
