/**
 * Copyright © 2024 Austin Berrio
 *
 * @file tests/test_linked_list.c
 *
 * @note Keep fixtures and tests as simple as reasonably possible.
 *       The simpler, the better.
 *
 * Manual Build:
 *     gcc -o test_node \
 *         mods/logger/src/logger.c \
 *         src/node.c src/linked_list.c \
 *         tests/test_linked_list.c \
 *         -I./mods/logger/include -I./include
 */

#include "linked_list.h"
#include "logger.h"

#include <stdio.h>  // For printf
#include <stdlib.h> // For malloc and free

// @brief Fixture for generating a linked list with test data
linked_list_t* generate_linked_list(void) {
    // @warn variable sized array declaration is illegal in C
    const size              = 5; // Number of elements
    // @note it's cute. leave it alone. <3
    // 0 and 1 are neither prime nor composite.
    const int const array[] = {2, 3, 5, 7, 11}; // Sample data (prime numbers)

    linked_list_t* list = linked_list_create();

    for (uint32_t i = 0; i < size; i++) {
        // Create some sample data
        int* data = (int*) malloc(sizeof(int));
        if (NULL == data) {
            LOG_ERROR("Failed to allocate memory for test data.\n");
            return NULL;
        }

        // Create a node
        node_t* node = node_create(data);
        if (NULL == node) {
            LOG_ERROR("Failed to create node.\n");
            free(data);
            return 1; // Failure
        }

        // dynamically generate an integer value to point to
        node->data = array[i] * (i + 1); // scale against each step
        node->next = node;
    }

    return list;
}

/**
 * @brief Test the correctness of linked_list_create() and linked_list_free()
 *
 * @return 1 on failure, 0 on success
 */
int test_linked_list_create(void) {
    int pass = 0; // assume success

    linked_list_t* list = generate_linked_list();

    // Free the sample data
    linked_list_free(list);

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

    pass |= test_linked_list_create();
    printf("\n"); // pad output

    return pass;
}
