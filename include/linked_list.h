/**
 * Copyright © 2024 Austin Berrio
 *
 * @file include/linked_list.h
 */

#ifndef DSA_LINKED_LIST_H
#define DSA_LINKED_LIST_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "node.h"

#include <stdbool.h> // For using bool type
#include <stdint.h>

/**
 * @brief A sequence of linearly ordered objects.
 *
 * @param head Pointer to the first node in the list.
 * @param size Keeps track of the number of elements.
 */
typedef struct LinkedList {
    node_t*  head; // Pointer to the first node in the list
    uint32_t size; // Keeps track of the number of elements
} linked_list_t;

/**
 * @brief Initialize a new linked list.
 *
 * @return A pointer to the newly created linked list.
 */
linked_list_t* linked_list_create(void);

/**
 * @brief Free a linked list and all its nodes.
 *
 * @param list Pointer to the linked list to be freed.
 */
void linked_list_free(linked_list_t* list);

/**
 * @brief Add a node to the end of a list.
 *
 * @param list Pointer to the linked list.
 * @param data Pointer to the data to be stored in the new node.
 */
void linked_list_append(linked_list_t* list, void* data);

/**
 * @brief Add a node to the beginning of a list.
 *
 * @param list Pointer to the linked list.
 * @param data Pointer to the data to be stored in the new node.
 */
void linked_list_prepend(linked_list_t* list, void* data);

/**
 * @brief Insert a node at a given index in the list.
 *
 * @param list Pointer to the linked list.
 * @param index Position where the new node will be inserted.
 * @param data Pointer to the data to be stored in the new node.
 */
void linked_list_insert(linked_list_t* list, uint32_t index, void* data);

/**
 * @brief Remove the first node with matching data.
 *
 * @param list Pointer to the linked list.
 * @param data Pointer to the data to be matched for removal.
 */
void linked_list_remove(linked_list_t* list, void* data);

/**
 * @brief Get the number of nodes in the list.
 *
 * @param list Pointer to the linked list.
 * @return The number of nodes in the list.
 */
uint32_t linked_list_size(const linked_list_t* list);

/**
 * @brief Check if the list is empty.
 *
 * @param list Pointer to the linked list.
 * @return True if the list is empty, false otherwise.
 */
bool linked_list_is_empty(const linked_list_t* list);

/**
 * @brief Comparison function type for linked list operations.
 *
 * This function should return:
 * - A negative value if the first argument is less than the second.
 * - Zero if the first argument is equal to the second.
 * - A positive value if the first argument is greater than the second.
 */
typedef int (*compare_t)(const void*, const void*);

/**
 * @brief Find the first node containing the matching data.
 *
 * @param list Pointer to the linked list.
 * @param data Pointer to the data to search for.
 * @param compare A custom comparison function.
 * @return A pointer to the found node, or NULL if not found.
 */
node_t* linked_list_find(
    const linked_list_t* list, const void* data, compare_t compare
);

/**
 * @brief Remove and return the last node in the list.
 *
 * @param list Pointer to the linked list.
 * @return Data pointer from the removed node, or NULL if the list is empty.
 */
void* linked_list_pop_last(linked_list_t* list);

/**
 * @brief Remove and return the first node in the list.
 *
 * @param list Pointer to the linked list.
 * @return Data pointer from the removed node, or NULL if the list is empty.
 */
void* linked_list_pop_first(linked_list_t* list);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DSA_LINKED_LIST_H
