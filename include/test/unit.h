/**
 * Copyright © 2023 Austin Berrio
 *
 * @file include/test/unit.h
 * @brief Minimal customizable unit testing framework for C.
 *
 * Provides structures and functions to define, run, and manage unit tests
 * with flexible test logic and callback hooks.
 */

#ifndef TEST_UNIT_H
#define TEST_UNIT_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stddef.h>
#include <stdint.h>

/**
 * @name Macros
 * @{
 */

/**
 * @brief Asserts a condition in a test and logs an error if false.
 *
 * If the condition is false, logs the formatted error message and
 * returns 1 to indicate test failure.
 *
 * @param condition Condition to assert.
 * @param format printf-style format string for the error message.
 * @param ... Arguments for the format string.
 */
#define ASSERT(condition, format, ...) \
    do { \
        if (!(condition)) { \
            LOG_ERROR(format, ##__VA_ARGS__); \
            return 1; \
        } \
    } while (0)

/** @} */

/**
 * @name Forward Declarations
 * @{
 */

/**
 * @brief Structure representing a unit test.
 *
 * This structure is used to define individual unit tests,
 * including before, run, and after hooks.
 */
typedef struct TestUnit TestUnit;

/** @} */

/**
 * @name Function Pointer Types
 * @{
 */

/**
 * @brief Pointer type for group test functions.
 *
 * Functions implementing a logical unit test should take a pointer to a
 * TestUnit and return 0 for success, non-zero for failure.
 */
typedef int (*TestHook)(TestUnit* unit);

/**
 * @brief Pointer type for suite test functions.
 *
 * Test suites run a series of tests and return 0 on success, non-zero otherwise.
 */
typedef int (*TestFunction)(void);

/** @} */

/**
 * @name Structures
 * @{
 */

/**
 * @brief Represents a individual unit test.
 */
typedef struct TestUnit {
    int8_t result; /**< Result of the test unit (0 = success, 1 = failure). */
    size_t index; /**< Index number of the test unit (1-based). */
    const void* data; /**< User-defined parameters. */
} TestUnit;

/**
 * @brief Context for running a group of tests.
 */
typedef struct TestGroup {
    const char* name; /**< Name of the unit test group. */
    size_t count; /**< Number of unit tests in the group. */
    TestUnit* units; /**< Array of unit tests to run. */
    TestHook run; /**<  Hook for running unit tests. */
    TestHook before; /**< (Optional) Hook to execute before unit tests. */
    TestHook after; /**< (Optional) Hook to execute after unit tests. */
} TestGroup;

/**
 * @brief Represents a named test suite.
 */
typedef struct TestSuite {
    const char* name; /**< Name of the test suite. */
    TestFunction run; /**< Function to run the test suite. */
} TestSuite;

/** @} */

/**
 * @name Unit Test Functions
 * @{
 */

/**
 * @brief Runs a group of unit tests.
 *
 * Executes the provided TestHook function on each TestUnit.
 * Optionally invokes a callback before and or after each test.
 * Logs results and returns 0 if all tests pass, 1 otherwise.
 *
 * @param group Pointer to the TestGroup defining the tests.
 * @return 0 if all tests pass, 1 if any fail, -1 on invalid input.
 */
int test_group_run(TestGroup* group);

/**
 * @brief Runs a named test suite.
 *
 * Logs start and completion status of the suite.
 *
 * @param suite Pointer to the TestSuite defining the tests.
 * @return 0 on success, non-zero on failure.
 */
int test_suite_run(TestSuite* suite);

/** @} */

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TEST_UNIT_H
