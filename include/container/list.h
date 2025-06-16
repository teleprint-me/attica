/**
 * Copyright © 2023 Austin Berrio
 *
 * @file include/container/list.h
 */

#ifndef DSA_CONTAINER_LIST_H
#define DSA_CONTAINER_LIST_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "container/node.h"

#include <stdbool.h> // For using bool type
#include <stdint.h>

/**
 * @brief A sequence of linearly ordered objects.
 *
 * @param head Pointer to the first node in the list.
 * @param size Keeps track of the number of elements.
 */
typedef Container ContainerList;

/**
 * @brief Comparison function type for linked list operations.
 *
 * This function should return:
 * - A negative value if the first argument is less than the second.
 * - Zero if the first argument is equal to the second.
 * - A positive value if the first argument is greater than the second.
 */
typedef int (*ContainerListCompare)(const void*, const void*);

/**
 * @brief Default comparison function for numeric data.
 *
 * @param a Pointer to the first number.
 * @param b Pointer to the second number.
 * @return -1 if a < b, 0 if a == b, 1 if a > b.
 */
int container_list_numeric_compare(const void* a, const void* b);

/**
 * @brief Initialize a new linked list.
 *
 * @return A pointer to the newly created linked list.
 */
ContainerList* container_list_create(void);

/**
 * @brief Free a linked list, all its nodes, and optionally its associated data
 * using a custom cleanup function.
 *
 * @param list Pointer to the linked list to be freed.
 * @param callback (Optional) Function pointer to a user-defined cleanup
 * function for freeing node data. Pass NULL if no custom cleanup is needed,
 * free() for simple cleanup, or a custom function for more complex objects.
 *
 * This function will:
 * - Free the memory occupied by each node in the linked list
 * - If a callback function is provided, call it with the corresponding data
 *   pointer from each node before freeing its memory
 */
void container_list_free(ContainerList* list, void (*callback)(void*));

/**
 * @brief Add a node to the end of a list.
 *
 * @param list Pointer to the linked list.
 * @param data Pointer to the data to be stored in the new node.
 */
void container_list_append(ContainerList* list, void* data);

/**
 * @brief Insert a node at a given index in the list.
 *
 * @param list Pointer to the linked list.
 * @param data Pointer to the data to be stored in the new node.
 * @param index Position where the new node will be inserted.
 */
void container_list_insert(ContainerList* list, void* data, size_t index);

/**
 * @brief Get the number of nodes in the list.
 *
 * @param list Pointer to the linked list.
 * @return The number of nodes in the list.
 */
size_t container_list_size(const ContainerList* list);

/**
 * @brief Check if the list is empty.
 *
 * @param list Pointer to the linked list.
 * @return True if the list is empty, false otherwise.
 */
bool container_list_is_empty(const ContainerList* list);

/**
 * @brief Find the first node containing the matching data.
 *
 * @param list Pointer to the linked list.
 * @param data Pointer to the data to search for.
 * @param compare A custom comparison function.
 * @return A pointer to the found node, or NULL if not found.
 */
ContainerNode* container_list_find(
    const ContainerList* list, const void* data, ContainerListCompare compare
);

/**
 * @brief Remove the first node with matching data.
 *
 * @param list Pointer to the linked list.
 * @param data Pointer to the data to be matched for removal.
 */
void container_list_remove(
    ContainerList* list, void* data, ContainerListCompare compare
);

/**
 * @brief Remove and return the last node in the list.
 *
 * @param list Pointer to the linked list.
 * @return Data pointer from the removed node, or NULL if the list is empty.
 */
void* container_list_pop(ContainerList* list);

/**
 * @brief Remove and return a node by index from the list.
 *
 * @param list Pointer to the linked list.
 * @return Data pointer from the removed node, or NULL if the list is empty.
 */
void* container_list_pop_index(ContainerList* list, size_t index);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DSA_CONTAINER_LIST_H
