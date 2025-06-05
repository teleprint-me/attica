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

void test_node_setup(TestUnit* unit) {
    TestContainerNode* params = (TestContainerNode*) unit->params;
    params->node = container_node_create(&params->value);
}

void test_node_teardown(TestUnit* unit) {
    TestContainerNode* params = (TestContainerNode*) unit->params;
    container_node_free(params->node);
}

int test_container_node(TestUnit* unit) {
    TestContainerNode* params = (TestContainerNode*) unit->params;
    int result = *(int*) params->node->object;

    ASSERT(
        result == params->expected,
        "[TestContainerNode] expected %d, got %d",
        params->expected,
        result
    );

    return 0;
}

int test_container_node_suite(void) {
    TestContainerNode params[] = {
        {.value = 5, .expected = 5},
        {.value = 3, .expected = 3},
        {.value = 7, .expected = 7},
    };

    size_t count = sizeof(params) / sizeof(TestContainerNode);
    TestUnit units[count];
    for (size_t i = 0; i < (count); i++) {
        (units)[i].params = &(params)[i];
    }

    TestGroup group = {
        .name = "Test Node Group",
        .count = count,
        .units = units,
        .run = test_container_node,
        .before = test_node_setup,
        .after = test_node_teardown,
    };

    return test_group_run(&group);
}

int main(void) {
    TestSuite suites[] = {
        {"Test Node Suite", test_container_node_suite},
    };

    int result = 0;
    size_t total = sizeof(suites) / sizeof(TestSuite);
    for (size_t i = 0; i < total; i++) {
        result |= test_suite_run(&suites[i]);
    }

    return result;
}
