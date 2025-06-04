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
 * @brief Structure representing a test case.
 *
 * This structure is used to define individual test cases,
 * including setup, logic, and teardown hooks.
 */
typedef struct TestCase TestCase;

/** @} */

/**
 * @name Function Pointer Types
 * @{
 */

/**
 * @brief Hook type for test setup/teardown.
 *
 * Hooks are functions that are called before and after each test case.
 * They are useful for setting up and cleaning up resources.
 */
typedef void (*TestHook)(TestCase* test);

/**
 * @brief Pointer type for test case functions.
 *
 * Functions implementing the logical test case should take a pointer to a
 * TestCase and return 0 for success, non-zero for failure.
 */
typedef int (*TestCallback)(TestCase* test);

/**
 * @brief Pointer type for test suite functions.
 *
 * Test suites run a series of tests and return 0 on success, non-zero otherwise.
 */
typedef int (*TestSuite)(void);

/** @} */

/**
 * @name Structures
 * @{
 */

/**
 * @brief Represents a single test case.
 */
typedef struct TestCase {
    int8_t result; /**< Result of the test case (0 = success, 1 = failure). */
    size_t index; /**< Index number of the test case (1-based). */
    const void* unit; /**< Pointer to user-defined test data or state. */
} TestCase;

/**
 * @brief Context for running a group of tests.
 */
typedef struct TestContext {
    const char* name; /**< Name or description of the test suite. */

    size_t total; /**< Total number of test cases in the context. */
    TestCase* cases; /**< Array of test cases to run. */
    TestCallback callback; /**< Function pointer to logical test case implementation. */

    TestHook setup; /**< (Optional) Function pointer to test case setup. */
    TestHook teardown; /**< (Optional) Function pointer to test case teardown. */
} TestContext;

/**
 * @brief Represents a named test suite.
 */
typedef struct TestRegistry {
    const char* name; /**< Name of the test suite. */
    TestSuite suite; /**< Pointer to the function that runs the suite. */
} TestRegistry;

/** @} */

/**
 * @name Unit Test Functions
 * @{
 */

/**
 * @brief Runs a set of unit tests within a TestContext.
 *
 * Executes the provided TestLogic function on each TestCase.
 * Optionally invokes a callback after each test.
 * Logs results and returns 0 if all tests pass, 1 otherwise.
 *
 * @param context Pointer to the TestContext describing the tests.
 * @return 0 if all tests pass, 1 if any fail, -1 on invalid input.
 */
int test_unit_run(TestContext* context);

/**
 * @brief Runs a named test suite function.
 *
 * Logs start and completion status of the suite.
 *
 * @param name Name of the test suite.
 * @param suite Function pointer to the test suite to run.
 * @return 0 on success, non-zero on failure.
 */
int test_suite_run(const char* name, TestSuite suite);

/** @} */

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TEST_UNIT_H
