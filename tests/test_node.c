/**
 * Copyright © 2024 Austin Berrio
 *
 * @file tests/test_node.c
 *
 * @note Keep fixtures and tests as simple as reasonably possible.
 *       The simpler, the better.
 *
 * Manual Build:
 *     gcc -o test_node \
 *         mods/logger/src/logger.c \
 *         src/node.c tests/test_node.c \
 *         -I./mods/logger/include -I./include
 */

#include "logger.h"
#include "node.h"

#include <stdio.h>  // For printf
#include <stdlib.h> // For malloc and free

/**
 * @brief Test the correctness of node_create and node_free
 *
 * @return 1 on failure, 0 on success
 */
int test_node_create(void) {
    int pass = 0; // assume success

    // Create some sample data
    int* data = (int*) malloc(sizeof(int));
    if (NULL == data) {
        LOG_ERROR("Failed to allocate memory for test data.\n");
        return 1; // Failure
    }
    *data = 42;

    // Create a node
    node_t* node = node_create(data);
    if (NULL == node) {
        LOG_ERROR("Failed to create node.\n");
        free(data);
        return 1; // Failure
    }

    // Check that the data is correctly set
    if (node->data != data) {
        LOG_ERROR("Node data does not match expected value.\n");
        pass = 1;
    }

    // Free the node (not the data)
    node_free(node);

    // Free the sample data
    free(data);

    printf("%s", 0 == pass ? "." : "x");
    return pass;
}

/**
 * @brief Test the correctness of allocating and deallocating data
 *
 * @return 1 on failure, 0 on success
 */
int test_node_data_allocation(void) {
    int pass = 0; // assume success

    // Create some sample data
    double* data = (double*) malloc(sizeof(double));
    if (NULL == data) {
        LOG_ERROR("Failed to allocate memory for test data.\n");
        return 1; // Failure
    }
    *data = 3.14;

    // Create a node
    node_t* node = node_create(data);
    if (NULL == node) {
        LOG_ERROR("Failed to create node.\n");
        free(data);
        return 1; // Failure
    }

    // Check that the node's data is correctly set
    if (*(double*) (node->data) != 3.14) {
        LOG_ERROR("Node data does not match expected value (3.14).\n");
        pass = 1;
    }

    // Free the node
    node_free(node);

    // Free the sample data
    free(data);

    printf("%s", 0 == pass ? "." : "x");
    return pass;
}

/**
 * @brief Main function to run all unit tests.
 *
 * @return 1 on failure, 0 on success
 */
int main(void) {
    int pass = 0; // assume success

    pass |= test_node_create();
    pass |= test_node_data_allocation();
    printf("\n"); // pad output

    return pass;
}
