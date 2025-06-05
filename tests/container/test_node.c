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
    TestContainerNode* data = (TestContainerNode*) unit->data;
    data->node = container_node_create(&data->value);
}

void test_node_teardown(TestUnit* unit) {
    TestContainerNode* data = (TestContainerNode*) unit->data;
    container_node_free(data->node);
}

int test_container_node(TestUnit* unit) {
    TestContainerNode* data = (TestContainerNode*) unit->data;
    int result = *(int*) data->node->object;

    ASSERT(
        result == data->expected,
        "[TestContainerNode] expected %d, got %d",
        data->expected,
        result
    );

    return 0;
}

int test_container_node_suite(void) {
    TestContainerNode data[] = {
        {.value = 5, .expected = 5},
        {.value = 3, .expected = 3},
        {.value = 7, .expected = 7},
    };

    size_t count = sizeof(data) / sizeof(TestContainerNode);
    TestUnit units[count];
    for (size_t i = 0; i < (count); i++) {
        (units)[i].data = &(data)[i];
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
