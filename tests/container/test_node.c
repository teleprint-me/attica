/**
 * @file tests/container/test_node.c
 */

#include "core/logger.h"
#include "test/unit.h"
#include "container/node.h"

typedef struct TestContainerNode {
    int value;
    int expected;
    ContainerNode* node;
} TestContainerNode;

void test_node_setup(TestCase* test) {
    TestContainerNode* unit = (TestContainerNode*) test->unit;
    unit->node = container_node_create(&unit->value);
}

void test_node_teardown(TestCase* test) {
    TestContainerNode* unit = (TestContainerNode*) test->unit;
    container_node_free(unit->node);
}

int test_container_node(TestCase* test) {
    TestContainerNode* unit = (TestContainerNode*) test->unit;
    int result = *(int*) unit->node->object;

    ASSERT(
        result == unit->expected, "[TestContainerNode] expected %d, got %d", unit->expected, result
    );

    return 0;
}

int test_container_node_suite(void) {
    TestContainerNode cases[] = {
        {.value = 5, .expected = 5},
        {.value = 3, .expected = 3},
        {.value = 7, .expected = 7},
    };

    size_t total_tests = sizeof(cases) / sizeof(TestContainerNode);
    TestCase test_cases[total_tests];
    for (size_t i = 0; i < (total_tests); i++) {
        (test_cases)[i].unit = &(cases)[i];
    }

    TestContext context = {
        .total_tests = total_tests,
        .test_name = "Test Node",
        .test_cases = test_cases,
        .setup_each = test_node_setup,
        .teardown_each = test_node_teardown,
    };

    int result = test_unit_run(&context, test_container_node, NULL);

    return result;
}

int main(void) {
    static TestRegister suites[] = {
        {"Container Node", test_container_node_suite},
    };

    int result = 0;
    size_t total = sizeof(suites) / sizeof(TestRegister);
    for (size_t i = 0; i < total; i++) {
        result |= test_suite_run(suites[i].name, suites[i].test_suite);
    }

    return result;
}
