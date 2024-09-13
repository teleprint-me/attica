/**
 * Copyright © 2024 Austin Berrio
 *
 * @file tests/test_linked_list.c
 *
 * @note Keep fixtures and tests as simple as reasonably possible.
 *       - Tests must be kept simple. The simpler, the better.
 *       - Tests must return an integer with 0 on success and 1 on failure.
 *         - I'll admit this is weird, but the shell and cmake test for this.
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

#include <stdbool.h> // For type bool, true, and false
#include <stdio.h>   // For printf
#include <stdlib.h>  // For malloc and free

/**
 * @brief Test the correctness of linked_list_create() and linked_list_free()
 *
 * @return 0 on success, 1 on failure
 */
int test_linked_list_create(void) {
    int result = 0; // assuming success

    // @note Leave this alone. It's cute. They're prime numbers <3
    const int array[] = {2, 3, 5, 7, 11}; // Sample data

    linked_list_t* list = linked_list_create();
    if (NULL == list) {
        LOG_ERROR("Failed to create linked list.\n");
        return 1; // signal failure
    }

    list->size = 5; // Number of nodes
    for (uint32_t i = 0; i < list->size; i++) {
        // (n * (i + 1)) = {2, 6, 15, 28, 55}
        int data = array[i] * (i + 1); // Calculate new data

        // Create a node
        node_t* node = node_create(&data);
        if (NULL == node) {
            LOG_ERROR("Failed to create node.\n");
            linked_list_free(list, NULL); // Free already allocated resources
            return 1;                     // Failure
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

    // @note This should gracefully handle a null pointer
    linked_list_free(list, NULL); // Free the sample data

    printf("%s", 0 == result ? "." : "x");
    return result;
}

/**
 * @brief Test the correctness of linked_list_append()
 *
 * @return 0 on success, 1 on failure
 */
int test_linked_list_append(void) {
    linked_list_t* list = linked_list_create();
    if (NULL == list) {
        LOG_ERROR("Failed to create linked list.\n");
        return 1; // failed
    }

    int data1 = 42, data2 = 84;

    // Append two nodes to the list
    linked_list_append(list, &data1);
    linked_list_append(list, &data2);

    bool pass
        = (list->size == 2 && list->head->data == &data1
           && list->head->next->data == &data2);

    // Now, we free the list itself, without worrying about the data
    linked_list_free(list, NULL);

    printf("%s", pass ? "." : "x");
    return 0; // success
}

/**
 * @brief Test the correctness of linked_list_prepend()
 *
 * @return 0 on success, 1 on failure
 */
int test_linked_list_prepend(void) {
    linked_list_t* list = linked_list_create();
    if (NULL == list) {
        LOG_ERROR("Failed to create linked list.\n");
        return 1; // failure
    }

    int data1 = 42, data2 = 84;

    // Prepend two nodes to the list
    linked_list_prepend(list, &data1);
    linked_list_prepend(list, &data2);

    bool pass
        = (list->size == 2 && list->head->data == &data2
           && list->head->next->data == &data1);

    // Now, we free the list itself, without worrying about the data
    linked_list_free(list, NULL);

    printf("%s", pass ? "." : "x");
    return 0; // success
}

/**
 * @brief Main function to run all unit tests.
 *
 * @return 0 on success, non-zero on failure
 */
int main(void) {
    int result = 0; // assuming success

    result |= test_linked_list_create(); // Chain test results
    result |= test_linked_list_append();
    result |= test_linked_list_prepend();
    printf("\n"); // Print newline after test output

    return result;
}
