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
    // @note it's cute. leave it alone. they're prime numbers <3
    const int const array[] = {2, 3, 5, 7, 11}; // Sample data

    linked_list_t* list = linked_list_create();
    if (NULL == list) {
        LOG_ERROR("Failed to create linked list.\n");
        return NULL;
    }

    list->size = 5; // Number of nodes
    for (uint32_t i = 0; i < list->size; i++) {
        // Allocate memory for new data
        int* data = (int*) malloc(sizeof(int));
        if (NULL == data) {
            LOG_ERROR("Failed to allocate memory for test data.\n");
            linked_list_free(list); // Free already allocated resources
            return NULL;
        }

        // @warn do not directly reference the arrays values
        *data = array[i] * (i + 1); // calculate new data

        // Create a node
        node_t* node = node_create(data);
        if (NULL == node) {
            LOG_ERROR("Failed to create node.\n");
            free(data);
            return 1; // Failure
        }

        // @note this is super inefficient, but is fine in this context
        // perf will be really bad with really big input sizes
        node_t* head = list->head;
        while (NULL != head) {
            head = head->next;
        }
        head = node;
    }

    return list;
}

/**
 * @brief Test the correctness of linked_list_create() and linked_list_free()
 *
 * @return True on success, false on failure
 */
bool test_linked_list_create(void) {
    linked_list_t* list = generate_linked_list();
    if (NULL == list) {
        LOG_ERROR("Failed to generate linked list.\n");
        return false;
    }

    linked_list_free(list); // Free the sample data

    printf("."); // Test passed indicator
    return true; // Test succeeded
}

/**
 * @brief Main function to run all unit tests.
 *
 * @return 0 on success, non-zero on failure
 */
int main(void) {
    bool all_passed = true;

    all_passed &= test_linked_list_create(); // Chain test results
    printf("\n");                            // Print newline after test output

    return all_passed ? 0 : 1; // Return appropriate exit status
}
