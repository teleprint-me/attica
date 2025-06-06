/**
 * Copyright © 2023 Austin Berrio
 *
 * @file include/container/node.h
 */

#ifndef DSA_CONTAINER_NODE_H
#define DSA_CONTAINER_NODE_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @name Node Structures
 * @{
 */

/**
 * @brief A point on a stem at which a leaf or leaves are inserted
 *
 * @param object Generic pointer to hold any type of object
 * @param next Pointer to the next node
 * @param prev Pointer to the previous node
 */
typedef struct ContainerNode {
    void* object; // Generic pointer to hold any type of object
    struct ContainerNode* next; // Pointer to the next node
    struct ContainerNode* prev; // Pointer to the previous node
} ContainerNode;

/**
 * @}
 */

/**
 * @name Node Functions
 * @{
 */

/**
 * @brief Function to create a new node.
 */
ContainerNode* container_node_create(void* object);

/**
 * @brief Function to free a node.
 */
void container_node_free(ContainerNode* node);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DSA_CONTAINER_NODE_H
