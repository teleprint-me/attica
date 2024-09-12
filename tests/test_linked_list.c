/**
 * Copyright © 2024 Austin Berrio
 *
 * @file tests/test_linked_list.c
 *
 * @note Keep fixtures and tests as simple as reasonably possible.
 *       The simpler, the better.
 *
 * Manual Build:
 *     gcc -o test_linked_list \
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
    // @note Leave this alone. It's cute. They're prime numbers <3
    const int array[] = {2, 3, 5, 7, 11}; // Sample data

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

        // @warn Do not point to the stack
        *data = array[i] * (i + 1); // Calculate new data

        // Create a node
        node_t* node = node_create(data);
        if (NULL == node) {
            LOG_ERROR("Failed to create node.\n");
            free(data);
            linked_list_free(list); // Free already allocated resources
            return NULL;            // Failure
        }

        // Manually append node to the end of the list
        if (NULL == list->head) {
            list->head = node; // First node becomes head
        } else {
            node_t* current = list->head;
            while (NULL != current->next) {
                current = current->next;
            }
            current->next = node; // Append at the end
        }
    }

    return list;
}

/**
 * @brief Test the correctness of linked_list_create() and linked_list_free()
 *
 * @return 0 on success, 1 on failure
 */
int test_linked_list_create(void) {
    int result = 0; // assuming success

    // get the pre-allocated linked list
    linked_list_t* list = generate_linked_list();
    if (NULL == list) {
        LOG_ERROR("Failed to generate linked list.\n");
        result = 1; // signal failure
    }

    // @note This should gracefully handle a null pointer
    linked_list_free(list); // Free the sample data

    printf("%s", 0 == result ? "." : "x");
    return result;
}

/**
 * @brief Main function to run all unit tests.
 *
 * @return 0 on success, non-zero on failure
 */
int main(void) {
    int passed = 0; // assuming success

    passed |= test_linked_list_create(); // Chain test results
    printf("\n");                        // Print newline after test output

    return passed;
}
