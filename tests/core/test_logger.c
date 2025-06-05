/**
 * Copyright © 2023 Austin Berrio
 *
 * @file tests/core/test_logger.c
 * @brief Automated unit tests for the thread-safe logger implementation.
 *
 * Manual Build:
 *   gcc -o test_logger src/core/logger.c src/test/unit.c tests/core/test_logger.c -I./include -lpthread
 *
 * Manual Run:
 *   ./test_logger
 *   # Displays detailed pass/fail status for each logger test suite.
 *
 * CMake Build:
 *   cmake -B build -DCMAKE_BUILD_TYPE=Debug
 *   cmake --build build --config Debug -j 16
 *
 * CMake Run (CTest):
 *   ctest --output-on-failure --test-dir build
 *   # Integrates with CMake/CTest; works in CI and locally.
 *
 * Output:
 *   ./build/tests/core/test_logger
 *   [INFO] ... [RUN] <suite>
 *   [INFO] ... [RESULT] <suite>: <N/N> tests passed
 *   [PASS]/[FAIL] suite status
 *   # See test_logger output for detailed logs and any assertion failures.
 *
 * Temp Files:
 *   Each test writes logs to 'temp.log' in the working directory for verification.
 *   Files are created and removed automatically; no manual cleanup required.
 *
 * Notes:
 *   - Tests cover: log level filtering, file logging, global logger, and lazy logger initialization.
 *   - No manual inspection required; all assertions are automated.
 *   - To check actual log file output, inspect 'temp.log' after manual test run.
 */

#include "core/logger.h"
#include "test/unit.h"

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

/**
 * @section File Utilities
 */

static const char* file_temp = "temp.log";

static int file_capture(FILE* pipe, const char* path) {
    remove(path); // Clean up BEFORE reading
    fflush(stderr);
    int state = dup(fileno(pipe));
    int file = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    dup2(file, fileno(pipe));
    close(file);
    return state;
}

static void file_restore(FILE* pipe, const char* path, int state) {
    fflush(pipe);
    dup2(state, fileno(pipe));
    close(state);
    remove(path); // Clean up AFTER reading
}

static bool file_match(const char* path, const char* message) {
    // Read back file and check if the message appears
    FILE* fp = fopen(path, "r");
    bool match = false;
    if (fp) {
        char line[512];
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, message)) {
                match = true;
                break;
            }
        }
        fclose(fp);
    }
    return match;
}

/**
 * @section Test Log Levels
 */

typedef struct TestLogLevel {
    const LogLevel log_level;
    const char* message;
    const bool expected; // was the message logged?
} TestLogLevel;

int test_group_log_level(TestUnit* unit) {
    TestLogLevel* data = (TestLogLevel*) unit->data;

    int state = file_capture(stderr, file_temp);

    Logger* logger = logger_create(LOG_LEVEL_WARN, LOG_TYPE_STREAM, NULL);
    if (!logger) {
        LOG_ERROR("[TestLogLevel] Failed to create a logger instance!");
        return 1;
    }
    const bool result = logger_message(logger, data->log_level, "%s\n", data->message);
    logger_free(logger);

    file_restore(stderr, file_temp, state);

    ASSERT(
        result == data->expected,
        "[TestLogLevel] log_level=%d, message=%d, expected='%s', got='%s'",
        data->log_level,
        data->message,
        data->expected ? "true" : "false",
        result ? "true" : "false"
    );

    return 0;
}

int test_suite_log_level(void) {
    TestLogLevel data[] = {
        {LOG_LEVEL_INFO, "This message should not appear", false},
        {LOG_LEVEL_WARN, "Global logger warning", true},
        {LOG_LEVEL_ERROR, "Global logger error", true},
    };

    size_t count = sizeof(data) / sizeof(TestLogLevel);
    TestUnit units[count];
    for (size_t i = 0; i < count; i++) {
        units[i].data = &data[i];
    }

    TestGroup group = {
        .name = "Test Log Level Group",
        .count = count,
        .units = units,
        .run = test_group_log_level,
    };

    return test_group_run(&group);
    ;
}

/**
 * @section Test Log Files
 */

typedef struct TestLogFile {
    LogLevel log_level;
    LogLevel message_level;
    const char* message;
    bool should_log; // Should this message appear in the file?
} TestLogFile;

int test_group_log_file(TestUnit* unit) {
    TestLogFile* data = (TestLogFile*) unit->data;

    int state = file_capture(stderr, file_temp);

    Logger* logger = logger_create(data->log_level, LOG_TYPE_FILE, file_temp);
    if (!logger) {
        LOG_ERROR("[TestLogFile] Failed to create a logger instance!");
        return 1;
    }
    logger_message(logger, data->message_level, "%s", data->message);
    logger_free(logger);

    const bool match = file_match(file_temp, data->message);
    file_restore(stderr, file_temp, state);

    ASSERT(
        match == data->should_log,
        "[TestLogFile] log_level=%d, message_level=%d, expected='%s', got='%s'",
        data->log_level,
        data->message_level,
        data->should_log ? "present" : "absent",
        match ? "present" : "absent"
    );

    return 0;
}

int test_suite_log_file(void) {
    TestLogFile data[] = {
        {LOG_LEVEL_INFO, LOG_LEVEL_DEBUG, "Debug should not log", false},
        {LOG_LEVEL_INFO, LOG_LEVEL_INFO, "Info should log", true},
        {LOG_LEVEL_INFO, LOG_LEVEL_ERROR, "Error should log", true},
    };

    size_t count = sizeof(data) / sizeof(TestLogFile);
    TestUnit units[count];
    for (size_t i = 0; i < count; i++) {
        units[i].data = &data[i];
    }

    TestGroup group = {
        .name = "Test Log File Group",
        .count = count,
        .units = units,
        .run = test_group_log_file,
    };

    return test_group_run(&group);
}

/**
 * @section Test Global Logger
 */

typedef struct TestGlobalLogger {
    LogLevel log_level; // What level is the global logger set to?
    LogLevel message_level; // What level is the log message?
    const char* message;
    bool should_log; // Should the message be logged, given the log_level?
} TestGlobalLogger;

int test_group_global_logger(TestUnit* unit) {
    TestGlobalLogger* data = (TestGlobalLogger*) unit->data;

    int state = file_capture(stderr, file_temp);

    // Set up global logger to desired level
    logger_set_global(data->log_level, LOG_TYPE_STREAM, "stream", stderr, NULL);

    // Use LOG_* macro (or direct logger_message for even tighter focus)
    LOG(&logger_global, data->message_level, "%s", data->message);

    const bool match = file_match(file_temp, data->message);
    file_restore(stderr, file_temp, state);

    logger_set_global(LOG_LEVEL_DEBUG, LOG_TYPE_STREAM, "stream", NULL, NULL); // Reset

    ASSERT(
        match == data->should_log,
        "[TestGlobalLogger] log_level=%d, message_level=%d, expected='%s', got='%s'",
        data->log_level,
        data->message_level,
        data->should_log ? "present" : "absent",
        match ? "present" : "absent"
    );

    return 0;
}

int test_suite_global_logger(void) {
    TestGlobalLogger data[] = {
        {LOG_LEVEL_WARN, LOG_LEVEL_INFO, "This message should not appear", false},
        {LOG_LEVEL_WARN, LOG_LEVEL_WARN, "Global logger warning", true},
        {LOG_LEVEL_WARN, LOG_LEVEL_ERROR, "Global logger error", true},
        {LOG_LEVEL_ERROR, LOG_LEVEL_WARN, "Warn should not log at error", false},
        {LOG_LEVEL_DEBUG, LOG_LEVEL_INFO, "Debug logger: info logs", true},
    };

    size_t count = sizeof(data) / sizeof(TestGlobalLogger);
    TestUnit units[count];
    for (size_t i = 0; i < count; i++) {
        units[i].data = &data[i];
    }

    TestGroup group = {
        .name = "Test Global Logger Group",
        .count = count,
        .units = units,
        .run = test_group_global_logger,
    };

    return test_group_run(&group);
}

/**
 * @section Test Lazy Initialization
 */

typedef struct TestLazyLogger {
    LogLevel log_level;
    LogLevel message_level;
    const char* message;
    bool should_log;
} TestLazyLogger;

int test_group_lazy_logger(TestUnit* unit) {
    TestLazyLogger* data = (TestLazyLogger*) unit->data;

    int state = file_capture(stderr, file_temp);

    Logger* lazy_logger = logger_create(data->log_level, LOG_TYPE_UNKNOWN, NULL);
    if (!lazy_logger) {
        LOG_ERROR("[TestLazyLogger] Failed to create a logger instance!");
        file_restore(stderr, file_temp, state);
        return 1;
    }
    LOG(lazy_logger, data->message_level, "%s", data->message);
    logger_free(lazy_logger);

    const bool match = file_match(file_temp, data->message);
    file_restore(stderr, file_temp, state);

    ASSERT(
        match == data->should_log,
        "[TestLazyLogger] log_level=%d, message_level=%d, expected='%s', got='%s'",
        data->log_level,
        data->message_level,
        data->should_log ? "present" : "absent",
        match ? "present" : "absent"
    );

    return 0;
}

int test_logger_lazy_suite(void) {
    TestLazyLogger data[] = {
        {LOG_LEVEL_DEBUG, LOG_LEVEL_DEBUG, "Lazy logger debug", true},
        {LOG_LEVEL_DEBUG, LOG_LEVEL_ERROR, "Lazy logger error", true},
        {LOG_LEVEL_INFO, LOG_LEVEL_DEBUG, "This should not log", false},
    };

    size_t count = sizeof(data) / sizeof(TestLazyLogger);
    TestUnit units[count];
    for (size_t i = 0; i < count; i++) {
        units[i].data = &data[i];
    }

    TestGroup group = {
        .name = "Test Lazy Logger Group",
        .count = count,
        .units = units,
        .run = test_group_lazy_logger,
    };

    return test_group_run(&group);
}

int main(void) {
    TestSuite suites[] = {
        {"Log Level Suite", test_suite_log_level},
        {"Log File Suite", test_suite_log_file},
        {"Global Logger Suite", test_suite_global_logger},
        {"Lazy Logger Suite", test_logger_lazy_suite},
    };

    int result = 0;
    size_t count = sizeof(suites) / sizeof(TestSuite);
    for (size_t i = 0; i < count; i++) {
        result |= test_suite_run(&suites[i]);
    }
    return result;
}
