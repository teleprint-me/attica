/**
 * Copyright © 2023 Austin Berrio
 *
 * @file include/container/node.h
 */

#ifndef DSA_CONTAINER_NODE_H
#define DSA_CONTAINER_NODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "core/posix.h"

#include <stddef.h>

/**
 * @name Container Structures
 * @{
 */

/**
 * @brief Generic doubly-linked node for container data structures.
 *
 * The index field is optional and may be ignored or managed by the integrator.
 * 
 * @note Users own the object and are responsible for cleanup.
 */
typedef struct MAX_ALIGN_ATTR ContainerNode {
    void* data; /**< Pointer to user data. */
    struct ContainerNode* next; /**< Next node in the container. */
    struct ContainerNode* prev; /**< Previous node in the container. */
    size_t index; /**< (Optional) Index of the node in the container. */
} ContainerNode;

/**
 * @brief Basic container header for collections.
 *
 * May be used for linked lists, queues, stacks, etc.
 */
typedef struct MAX_ALIGN_ATTR Container {
    ContainerNode* head; /**< Head node (may be NULL). */
    ContainerNode* tail; /**< Tail node (may be NULL). */
    size_t size; /**< Number of elements. */
} Container;

/**
 * @}
 */

/**
 * @name Container Functions
 */

/**
 * @brief Allocates and initializes a new node with the given object pointer.
 *
 * @param data Pointer to user data to store in the node.
 * @return Pointer to new node, or NULL on failure.
 */
ContainerNode* container_node_create(void* data);

/**
 * @brief Frees a node. Does not free the contained object.
 *
 * @param node Pointer to the node to free.
 */
void container_node_free(ContainerNode* node);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif // DSA_CONTAINER_NODE_H
