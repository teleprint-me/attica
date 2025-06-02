/**
 * Copyright © 2023 Austin Berrio
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
 * @brief Data structure representing the sampled data
 *
 * @param data The sampled data
 * @param size The number of data points
 */
typedef struct SampleData {
    int*     data; // The sampled data
    uint32_t size; // The number of data points
} sample_data_t;

/**
 * @brief Create an array of prime numbers as sample data.
 *
 * @param size Number of prime numbers to generate.
 * @return Pointer to a dynamically allocated sample_data_t structure.
 *
 * @note It's cute <3
 */
int* samples_create(uint32_t size) {
    if (0 == size) {
        LOG_ERROR("Sample size must be 1 or greater.\n");
        return NULL;
    }

    sample_data_t* samples = (sample_data_t*) malloc(sizeof(sample_data_t));
    samples->data          = (int*) malloc(size * sizeof(int));

    // @note Samples is a list of prime numbers <3
    for (uint32_t i = 2; i < size; i++) {
        // @note Test if prime once and only once
        if (0 == (i % 2)) {
            continue; // Skip non-prime values
        }
        // if is prime, typecast unsigned int to signed int
        samples->data[i] = (int) i;
    }

    return samples;
}

/**
 * @brief Free the allocated sample data.
 *
 * @param samples Pointer to the sample_data_t structure.
 */
void samples_free(sample_data_t* samples) {
    if (samples == NULL) {
        LOG_ERROR("Sample data is NULL.\n");
        return;
    }

    if (samples->data) {
        free(samples->data);
    }

    free(samples);
}

/**
 * @brief Test the correctness of linked_list_create() and linked_list_free()
 *
 * @return 0 on success, 1 on failure
 */
int test_linked_list_create(void) {
    linked_list_t* list = linked_list_create();
    if (NULL == list) {
        LOG_ERROR("Failed to create linked list.\n");
        return 1; // signal failure
    }

    const sample_data_t* samples = samples_create(5);
    list->size                   = samples->size;

    for (uint32_t i = 0; i < list->size; i++) {
        // Create a node
        node_t* node = node_create(&samples->data[i]);
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
    linked_list_free(list, NULL);
    samples_free(samples);

    printf(".");
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

    int* samples = samples_create(2);

    // Append two nodes to the list
    linked_list_append(list, &samples[0]);
    linked_list_append(list, &samples[1]);

    bool pass
        = (2 == list->size && &samples[0] == list->head->data
           && &samples[1] == list->head->next->data);

    // Now, we free the list itself, without worrying about the data
    linked_list_free(list, NULL);
    samples_free(samples);

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
    if (0 <= linked_list_numeric_compare(&a, &b)) {
        LOG_ERROR("Failed: expected a < b.\n");
        return 1;
    }

    // Test case: a == c
    if (0 != linked_list_numeric_compare(&a, &c)) {
        LOG_ERROR("Failed: expected a == c.\n");
        return 1;
    }

    // Test case: b > a
    if (0 >= linked_list_numeric_compare(&b, &a)) {
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
    if (2 != list->size
        || 0 != linked_list_numeric_compare(list->head->data, &data1)
        || 0 != linked_list_numeric_compare(list->head->next->data, &data3)) {
        LOG_ERROR("Failed: expected data2 to be removed.\n");
        linked_list_free(list, NULL);
        return 1;
    }

    // Test removal of the head node (data1)
    linked_list_remove(list, &data1, linked_list_numeric_compare);
    if (1 != list->size
        || 0 != linked_list_numeric_compare(list->head->data, &data3)) {
        LOG_ERROR("Failed: expected data1 to be removed.\n");
        linked_list_free(list, NULL);
        return 1;
    }

    // Test removal of the last remaining node (data3)
    linked_list_remove(list, &data3, linked_list_numeric_compare);
    if (0 != list->size || NULL != list->head) {
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

int test_linked_list_size(void) {
    linked_list_t* list = linked_list_create();
    if (NULL == list) {
        LOG_ERROR("Failed to create linked list.\n");
        return 1; // Failed
    }
    int data1 = 5, data2 = 10, data3 = 15;

    linked_list_append(list, &data1);
    linked_list_append(list, &data2);
    linked_list_append(list, &data3);

    if (3 == linked_list_size(list)) {
        LOG_ERROR("Failed: expected size to be 3.\n");
        linked_list_free(list, NULL);
        return 1;
    }

    // Test removal of the middle node (data2)
    linked_list_remove(list, &data2, linked_list_numeric_compare);

    if (2 == linked_list_size(list)) {
        LOG_ERROR("Failed: expected size to be 2.\n");
        linked_list_free(list, NULL);
        return 1;
    }

    return 0; // Success
}

int test_linked_list_is_empty(void) {
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
    result |= test_linked_list_numeric_compare();
    result |= test_linked_list_remove();
    printf("\n"); // Print newline after test output

    return result;
}
