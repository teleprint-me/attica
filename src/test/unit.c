/**
 * Copyright © 2023 Austin Berrio
 *
 * @file src/test/unit.c
 */

#include "core/logger.h"
#include "test/unit.h"

int test_group_run(TestGroup* group) {
    if (!group || !group->name || !group->units || group->count == 0 || !group->run) {
        LOG_ERROR("Invalid TestGroup parameters.");
        return -1;
    }

    LOG_INFO("[RUN] %s: Number of tests: %zu", group->name, group->count);

    size_t failures = 0;
    for (size_t i = 0; i < group->count; i++) {
        int result = 0;

        TestUnit* unit = &group->units[i];
        unit->index = i + 1;

        if (group->before) {
            result |= group->before(unit);
        }

        result |= group->run(unit);

        if (group->after) {
            result |= group->after(unit);
        }

        if (result != 0) {
            failures++;
            LOG_ERROR("[FAIL] %s: Test case %zu failed.", group->name, unit->index);
        }
    }

    size_t passed = group->count - failures;
    LOG_INFO("[RESULT] %s: %zu/%zu tests passed", group->name, passed, group->count);

    return failures > 0 ? 1 : 0;
}

int test_suite_run(TestSuite* suite) {
    if (!suite || !suite->name || !suite->run) {
        LOG_ERROR("Invalid TestSuite parameters.");
        return -1;
    }

    LOG_INFO("[RUN] %s", suite->name);

    int result = suite->run();
    if (result == 0) {
        LOG_INFO("[PASS] %s", suite->name);
    } else {
        LOG_ERROR("[FAIL] %s", suite->name);
    }
    return result;
}
