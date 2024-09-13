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

    // @note This should gracefully handle list as a null pointer
    linked_list_free(list, NULL); // Free the sample data

    printf("%s", (1) ? "." : "x");
    return 0;
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
 * @brief Test the correctness of linked_list_insert()
 *
 * @return 0 on success, 1 on failure
 */
int test_linked_list_insert(void) {
    int result = 0; // Assume success

    linked_list_t* list = linked_list_create();
    if (NULL == list) {
        LOG_ERROR("Failed to create linked list.\n");
        return 1; // Failure
    }

    // Sample data
    int data1 = 10, data2 = 20, data3 = 30, data4 = 40;

    // Test 1: Insert at the beginning
    linked_list_insert(list, &data1, 0);
    if (list->size != 1 || list->head->data != &data1) {
        LOG_ERROR("Test 1 failed: Incorrect insertion at the beginning.\n");
        result = 1;
    }

    // Test 2: Insert at the end
    linked_list_insert(list, &data2, 1);
    if (list->size != 2 || list->head->next->data != &data2) {
        LOG_ERROR("Test 2 failed: Incorrect insertion at the end.\n");
        result = 1;
    }

    // Test 3: Insert in the middle
    linked_list_insert(list, &data3, 1); // Insert at index 1
    if (list->size != 3 || list->head->next->data != &data3
        || list->head->next->next->data != &data2) {
        LOG_ERROR("Test 3 failed: Incorrect insertion in the middle.\n");
        result = 1;
    }

    // Test 4: Attempt to insert out-of-bounds
    linked_list_insert(list, &data4, 5); // Invalid index
    if (list->size != 3) {               // Size should remain unchanged
        LOG_ERROR("Test 4 failed: Out-of-bounds insertion should not change "
                  "the size.\n");
        result = 1;
    }

    // Clean up
    linked_list_free(list, NULL);

    printf("%s", result == 0 ? "." : "x");
    return result;
}

/**
 * @brief Test the correctness of linked_list_numeric_compare().
 *
 * @return 0 on success, 1 on failure
 */
int test_linked_list_numeric_compare(void) {
    int a = 5, b = 10, c = 5;

    // Test case: a < b
    if (linked_list_numeric_compare(&a, &b) >= 0) {
        LOG_ERROR("Failed: expected a < b.\n");
        return 1;
    }

    // Test case: a == c
    if (linked_list_numeric_compare(&a, &c) != 0) {
        LOG_ERROR("Failed: expected a == c.\n");
        return 1;
    }

    // Test case: b > a
    if (linked_list_numeric_compare(&b, &a) <= 0) {
        LOG_ERROR("Failed: expected b > a.\n");
        return 1;
    }

    printf(".");
    return 0; // Success
}

/**
 * @brief Test the correctness of linked_list_remove().
 *
 * @return 0 on success, 1 on failure
 */
int test_linked_list_remove(void) {
    linked_list_t* list = linked_list_create();
    if (NULL == list) {
        LOG_ERROR("Failed to create linked list.\n");
        return 1; // Failed
    }

    int data1 = 5, data2 = 10, data3 = 15;

    // Append nodes with data1, data2, data3
    linked_list_append(list, &data1);
    linked_list_append(list, &data2);
    linked_list_append(list, &data3);

    // Test removal of the middle node (data2)
    linked_list_remove(list, &data2, linked_list_numeric_compare);

    // Verify data2 is removed
    if (list->size != 2
        || linked_list_numeric_compare(list->head->data, &data1) != 0
        || linked_list_numeric_compare(list->head->next->data, &data3) != 0) {
        LOG_ERROR("Failed: expected data2 to be removed.\n");
        linked_list_free(list, NULL);
        return 1;
    }

    // Test removal of the head node (data1)
    linked_list_remove(list, &data1, linked_list_numeric_compare);
    if (list->size != 1
        || linked_list_numeric_compare(list->head->data, &data3) != 0) {
        LOG_ERROR("Failed: expected data1 to be removed.\n");
        linked_list_free(list, NULL);
        return 1;
    }

    // Test removal of the last remaining node (data3)
    linked_list_remove(list, &data3, linked_list_numeric_compare);
    if (list->size != 0 || list->head != NULL) {
        LOG_ERROR("Failed: expected data3 to be removed.\n");
        linked_list_free(list, NULL);
        return 1;
    }

    // Test removal of non-existent data
    linked_list_remove(
        list, &data1, linked_list_numeric_compare
    ); // Should not crash
    if (list->size != 0) {
        LOG_ERROR(
            "Failed: expected no change when removing non-existent data.\n"
        );
        linked_list_free(list, NULL);
        return 1;
    }

    linked_list_free(list, NULL);
    printf(".");
    return 0; // Success
}

/**
 * @brief Main function to run all unit tests.
 *
 * @return 0 on success, non-zero on failure
 */
int main(void) {
    int result = 0; // Assuming success

    // Chain test results
    result |= test_linked_list_create();
    result |= test_linked_list_append();
    result |= test_linked_list_prepend();
    result |= test_linked_list_insert();
    result |= test_linked_list_numeric_compare(); // Test comparison function
    result |= test_linked_list_remove();          // Test removal function
    printf("\n"); // Print newline after test output

    return result;
}
