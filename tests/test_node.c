/**
 * Copyright © 2024 Austin Berrio
 *
 * @file tests/test_node.c
 *
 * @note Keep fixtures and tests as simple as reasonably possible.
 *       The simpler, the better.
 *
 * Manual Build:
 *     gcc -o test_linear_scalar \
 *         mods/logger/src/logger.c \
 *         node.c tests/test_node.c \
 *         -I./include -I./mods/logger/include
 */

#include "logger.h"
#include "node.h"

/**
 * @brief Test scalar operations
 *
 * @return 1 on failure, 0 on success
 */

/**
 * @brief Test the correctness of node_create and node_free
 */
int test_node_create(void);

/**
 * @brief Test the correctness of allocating and deallocating data
 */
int test_node_data_allocation(void);

/**
 * @brief Unit Tests
 */

int test_scalar_add(void) {
    int pass = 0; // assume success

    printf("%s", 0 == pass ? "." : "x");
    return pass;
}

int test_scalar_subtract(void) {
    int pass = 0; // assume success

    int32_t        a      = 7;
    int32_t        b      = 2;
    int32_t        result = 0;
    numeric_data_t type   = NUMERIC_INT32;

    result = *(int32_t*) scalar_subtract(&a, &b, &result, type);

    if (0 == result) {
        LOG_ERROR("Failed to subtract result.\n");
        pass = 1;
    } else if (5 != result) {
        LOG_ERROR("Failed to get expected difference.\n");
    }

    printf("%s", 0 == pass ? "." : "x");
    return pass;
}

int main(void) {
    int pass = 1; // assume failure

    pass &= test_node_create();
    pass &= test_node_data_allocation();
    printf("\n"); // pad output

    return pass;
}
