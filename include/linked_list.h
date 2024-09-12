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

#include <stdint.h>

/**
 * @brief A sequence of linearly ordered objects
 *
 * @param head Pointer to the first node in the list
 * @param size Keeps track of the number of elements
 */
typedef struct LinkedList {
    node_t*  head; // Pointer to the first node in the list
    uint32_t size; // Keeps track of the number of elements
} linked_list_t;

/**
 * @brief Initialize a new linked list.
 */
linked_list_t* linked_list_create(uint32_t size);

/**
 * @brief Free a linked list.
 */
void linked_list_free(linked_list_t* list);

/**
 * @brief Add a node to the end of a list.
 */
void linked_list_append(linked_list_t* list, void* data);

/**
 * @brief Insert a node at a given index to a list.
 */
void linked_list_insert(linked_list_t* list, uint32_t index, void* data);

/**
 * @brief Remove the first node with matching data.
 */
void linked_list_remove(linked_list_t* list, void* data);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DSA_LINKED_LIST_H
