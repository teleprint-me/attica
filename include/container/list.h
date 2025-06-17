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
 */
void container_list_free(ContainerList* list);

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
 * @brief Add a node to the end of a list.
 *
 * @param list Pointer to the linked list.
 * @param data Pointer to the data to be stored in the new node.
 */
bool container_list_append(ContainerList* list, void* data);

/**
 * @brief Insert a node at a given index in the list.
 *
 * @param list Pointer to the linked list.
 * @param data Pointer to the data to be stored in the new node.
 * @param index Position where the new node will be inserted.
 */
bool container_list_insert(ContainerList* list, void* data, size_t index);

/**
 * @brief Remove a node at a given index from the list.
 *
 * @param list Pointer to the linked list.
 * @param index Position of the node to be removed.
 * @return A pointer to the removed node, or NULL if the index is out of bounds.
 */
ContainerNode* container_list_search(const ContainerList* list, size_t index);

/**
 * @brief Find the first node containing the matching data.
 *
 * @param list Pointer to the linked list.
 * @param data Pointer to the data to search for.
 * @return A pointer to the found node, or NULL if not found.
 */
ContainerNode* container_list_find(const ContainerList* list, const void* data);

/**
 * @brief Remove the first node with matching data.
 *
 * @param list Pointer to the linked list.
 * @param data Pointer to the data to be matched for removal.
 */
bool container_list_remove(ContainerList* list, void* data);

/**
 * @brief Remove and return the last item in the list.
 *
 * @param list Pointer to the linked list.
 * @return Data pointer from the removed node, or NULL if the list is empty.
 */
void* container_list_pop(ContainerList* list);

/**
 * @brief Remove and return an item by index from the list.
 *
 * @param list Pointer to the linked list.
 * @return Data pointer from the removed node, or NULL if the list is empty.
 */
void* container_list_pop_index(ContainerList* list, size_t index);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DSA_CONTAINER_LIST_H
