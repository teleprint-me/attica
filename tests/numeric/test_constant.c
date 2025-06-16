/**
 * Copyright © 2023 Austin Berrio
 *
 * @file tests/test_numeric_constant.c
 *
 * @brief A simple test suite for test_numeric_constant.c in pure C
 *
 * @note The 53-bit significand precision gives from 15 to 17 significant
 *       decimal digits precision (2−53 ≈ 1.11 × 10−16).
 */

#include "core/logger.h"
#include "test/unit.h"
#include "numeric/constant.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct ConstantTestIsCloseDouble {
    double a;
    double b;
    int64_t significand;
    bool expected;
} ConstantTestIsCloseDouble;

int test_constant_is_close_double(TestCase* test) {
    ConstantTestIsCloseDouble* unit = (ConstantTestIsCloseDouble*) test->unit;

    bool result = is_close_double(unit->a, unit->b, unit->significand);

    ASSERT(
        result == unit->expected,
        "[ConstantTestIsCloseDouble] is_close_double(%.15f, %.15f, %zu) → expected=%s, got=%s",
        unit->a,
        unit->b,
        unit->significand,
        unit->expected ? "true" : "false",
        result ? "true" : "false"
    );

    return 0;
}

int test_constant_is_close_double_suite(void) {
    // In comparison operations, positive infinity is larger than all values
    // except itself and NaN, and negative infinity is smaller than all values
    // except itself and NaN. NaN is unordered: it is not equal to, greater
    // than, or less than anything, including itself.
    // sauce: https://www.gnu.org/software/libc/manual/html_node/Infinity-and-NaN.html
    const double expected = 0.053803;
    static ConstantTestIsCloseDouble cases[] = {
        // precision is within a range of 6 digits
        {0.053803, expected, 6, true}, // equal to
        {expected, 0.053721, 6, false}, // less than
        {expected, 0.053951, 6, false}, // greater than
        // Negative numbers
        {-0.053803, -expected, 6, true}, // equal to
        {-expected, -0.053721, 6, false}, // greater than
        {-expected, -0.053951, 6, false}, // less than
        // Inf and NaN cases
        {INFINITY, INFINITY, 6, true}, // INF is equal to itself
        {NAN, 0.0, 6, false}, // NaN is not equal to any object
        {NAN, NAN, 6, false}, // NaN is not equal to itself
        // Large numbers with significant precision
        {1e6, 1e6 + 1, 0, false}, // no precision
        {1e-6, 1e-6 + 1e-9, 6, true}, // Very close but small tolerance
        // Test large significand
        {123456789.123456, 123456789.123456, 15, true},
        // Test differentiated large significand
        {123456789.123456, 123456789.123457, 15, false},
        {1e-15, 2e-15, 15, true} // Small numbers with large tolerance
    };

    size_t total_tests = sizeof(cases) / sizeof(ConstantTestIsCloseDouble);

    TestCase test_cases[total_tests];
    for (size_t i = 0; i < total_tests; i++) {
        test_cases[i].unit = &cases[i];
    }

    TestContext context = {
        .total_tests = total_tests,
        .test_name = "Constant Test Is Close Double",
        .test_cases = test_cases,
    };

    return test_unit_run(&context, test_constant_is_close_double, NULL);
}

typedef struct ConstantTestIsCloseFloat {
    float a;
    float b;
    size_t significand;
    bool expected;
} ConstantTestIsCloseFloat;

int test_constant_is_close_float(TestCase* test) {
    ConstantTestIsCloseFloat* unit = (ConstantTestIsCloseFloat*) test->unit;

    bool result = is_close_float(unit->a, unit->b, unit->significand);

    ASSERT(
        result == unit->expected,
        "[ConstantTestIsCloseFloat] is_close_float(%.7f, %.7f, %zu) → expected=%s, got=%s",
        (double) unit->a,
        (double) unit->b,
        unit->significand,
        unit->expected ? "true" : "false",
        result ? "true" : "false"
    );

    return 0;
}

int test_constant_is_close_float_suite(void) {
    static const float expected = 0.053803f;

    static ConstantTestIsCloseFloat cases[] = {
        {0.053803f, expected, 6, true},
        {expected, 0.053721f, 6, false},
        {expected, 0.053951f, 6, false},

        {-0.053803f, -expected, 6, true},
        {-expected, -0.053721f, 6, false},
        {-expected, -0.053951f, 6, false},

        {INFINITY, INFINITY, 6, true},
        {NAN, 0.0f, 6, false},
        {NAN, NAN, 6, false},

        {1e6f, 1e6f + 1.0f, 0, false},
        {1e-6f, 1e-6f + 1e-8f, 6, true},

        {123456.1234f, 123456.1234f, 7, true},
        {123456.1234f, 123456.1235f, 7, false},

        {1e-6f, 2e-6f, 7, true}
    };

    size_t total_tests = sizeof(cases) / sizeof(ConstantTestIsCloseFloat);

    TestCase test_cases[total_tests];
    for (size_t i = 0; i < total_tests; i++) {
        test_cases[i].unit = &cases[i];
    }

    TestContext context = {
        .total_tests = total_tests,
        .test_name = "Constant Test Is Close Float",
        .test_cases = test_cases,
    };

    return test_unit_run(&context, test_constant_is_close_float, NULL);
}

int main(void) {
    static TestRegister suites[] = {
        {"Is Close Double", test_constant_is_close_double_suite},
        {"Is Close Float", test_constant_is_close_float_suite},
    };

    int result = 0;
    size_t total = sizeof(suites) / sizeof(TestRegister);
    for (size_t i = 0; i < total; i++) {
        result |= test_suite_run(suites[i].name, suites[i].test_suite);
    }

    return result;
}
