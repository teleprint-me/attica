/**
 * Copyright © 2023 Austin Berrio
 *
 * @file src/test/unit.c
 */

#include "core/logger.h"
#include "test/unit.h"

int test_unit_run(TestContext* context) {
    if (!context || !context->callback || !context->cases || context->total == 0) {
        LOG_ERROR("Invalid parameters.");
        return -1;
    }

    LOG_INFO("[RUN] %s: Number of tests: %zu", context->name, context->total);

    size_t failures = 0;

    for (size_t i = 0; i < context->total; i++) {
        TestCase* test_case = &context->cases[i];
        test_case->index = i + 1;

        if (context->setup) {
            context->setup(test_case);
        }

        int result = context->callback(test_case);

        if (context->teardown) {
            context->teardown(test_case);
        }

        if (result != 0) {
            failures++;
            LOG_ERROR("[FAIL] %s: Test case %zu failed.", context->name, test_case->index);
        }
    }

    size_t passed = context->total - failures;
    LOG_INFO("[RESULT] %s: %zu/%zu tests passed", context->name, passed, context->total);

    return failures > 0 ? 1 : 0;
}

int test_suite_run(const char* name, TestSuite suite) {
    LOG_INFO("[RUN] %s", name);
    int result = suite();
    if (result == 0) {
        LOG_INFO("[PASS] %s", name);
    } else {
        LOG_ERROR("[FAIL] %s", name);
    }
    return result;
}
