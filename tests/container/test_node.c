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

int test_group_node_setup(TestUnit* unit) {
    TestContainerNode* data = (TestContainerNode*) unit->data;
    data->node = container_node_create(&data->value);
    ASSERT(data->node, "Failed to setup node");
    return 0;
}

int test_group_node_teardown(TestUnit* unit) {
    TestContainerNode* data = (TestContainerNode*) unit->data;
    ASSERT(data->node, "Failed to reference node");
    container_node_free(data->node);
    return 0;
}

int test_group_container_node(TestUnit* unit) {
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

int test_suite_container_node(void) {
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
        .name = "Container Node",
        .count = count,
        .units = units,
        .run = test_group_container_node,
        .before_each = test_group_node_setup,
        .after_each = test_group_node_teardown,
    };

    return test_group_run(&group);
}

int main(void) {
    TestSuite suites[] = {
        {"Container Node", test_suite_container_node},
    };

    int result = 0;
    size_t total = sizeof(suites) / sizeof(TestSuite);
    for (size_t i = 0; i < total; i++) {
        result |= test_suite_run(&suites[i]);
    }

    return result;
}
