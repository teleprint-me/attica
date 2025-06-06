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

int test_stack_setup(TestUnit* unit) {
    TestContainerStack* d = (TestContainerStack*)unit->data;
    d->stack = container_stack_create();
    ASSERT(container_stack_push(d->stack, &(d->value)), "Failed to push value: %d", d);
    ASSERT(d->stack != NULL, "Failed to create stack");
    return 0;
}

int test_stack_teardown(TestUnit* unit) {
    TestContainerStack* d = (TestContainerStack*)unit->data;
    ASSERT(d->stack, "Stack is not initialized");
    container_stack_free(d->stack);
    return 0;
}

int test_group_container_stack(TestUnit* unit) {
    TestContainerStack* d = (TestContainerStack*)unit->data;

    // Push
    ASSERT(container_stack_push(d->stack, &(d->value)), "Failed to push value: %d", d->value);

    // Peek
    int* peeked = (int*)container_stack_peek(d->stack);
    ASSERT(peeked && *peeked == d->expected, "Peek mismatch: got %d, expected %d", peeked ? *peeked : -1, d->expected);

    // Pop
    int* popped = (int*)container_stack_pop(d->stack);
    ASSERT(popped && *popped == d->expected, "Pop mismatch: got %d, expected %d", popped ? *popped : -1, d->expected);

    // After pop, should be empty
    void* should_be_null = container_stack_peek(d->stack);
    ASSERT(should_be_null == NULL, "Stack not empty after pop");

    return 0;
}

int test_suite_container_stack(void) {
    TestContainerStack data[] = {
        { .value = 1, .expected = 1, .stack = NULL },
        { .value = 2, .expected = 2, .stack = NULL },
        { .value = -42, .expected = -42, .stack = NULL },
    };

    size_t count = sizeof(data) / sizeof(TestContainerStack);
    TestUnit units[count];
    for (size_t i = 0; i < count; i++) {
        units[i].data = &data[i];
    }

    TestGroup group = {
        .name = "ContainerStack",
        .count = count,
        .units = units,
        .run = test_group_container_stack,
        .before = test_stack_setup,
        .after = test_stack_teardown,
    };

    return test_group_run(&group);
}

int main(void) {
    TestSuite suites[] = {
        {"Container Stack", test_suite_container_stack},
    };

    int result = 0;
    size_t count = sizeof(suites) / sizeof(TestSuite);
    for (size_t i = 0; i < count; i++) {
        result |= test_suite_run(&suites[i]);
    }

    return result;
}
