/**
 * @file tests/container/test_node.c
 */

#include "core/logger.h"
#include "test/unit.h"
#include "container/node.h"

typedef struct TestContainerNode {
    ContainerNode* node;
    int expected;
} TestContainerNode;

int test_container_node_create(TestCase* test) {
    TestContainerNode* unit = (TestContainerNode*) test->unit;

    int result = *(int*) unit->node->object;

    ASSERT(
        result == unit->expected,
        "[TestContainerNode] node=%p, object=%p, expected=%d, got=%d",
        (void*)unit->node,
        unit->node->object,
        unit->expected,
        result
    );

    return 0;
}

int test_container_node_create_suite(void) {
    int a = 5;
    int b = 3;
    int c = 7;

    ContainerNode* na = container_node_create(&a);
    ContainerNode* nb = container_node_create(&b);
    ContainerNode* nc = container_node_create(&c);

    na->next = nb;
    nb->prev = na;
    nb->next = nc;
    nc->prev = nb;

    TestContainerNode cases[] = {
        { .node = na, .expected = a },
        { .node = nb, .expected = b },
        { .node = nc, .expected = c },
    };

    size_t total_tests = sizeof(cases) / sizeof(TestContainerNode);

    TestCase test_cases[total_tests];
    for (size_t i = 0; i < total_tests; i++) {
        test_cases[i].unit = &cases[i];
    }

    TestContext context = {
        .total_tests = total_tests,
        .test_name = "Test Container Node Create",
        .test_cases = test_cases,
    };

    int result = test_unit_run(&context, test_container_node_create, NULL);

    container_node_free(nc);
    container_node_free(nb);
    container_node_free(na);

    return result;
}

int main(void) {
    static TestRegister suites[] = {
        {"Container Node Create", test_container_node_create_suite},
    };

    int result = 0;
    size_t total = sizeof(suites) / sizeof(TestRegister);
    for (size_t i = 0; i < total; i++) {
        result |= test_suite_run(suites[i].name, suites[i].test_suite);
    }

    return result;
}