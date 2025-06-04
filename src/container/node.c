/**
 * Copyright © 2023 Austin Berrio
 *
 * @file src/container/node.c
 */

#include "core/logger.h"
#include "core/memory.h"

#include "container/node.h"

#include <stddef.h> // For NULL
#include <stdlib.h> // For malloc and free

ContainerNode* container_node_create(void* object) {
    // Allocate memory for the new node
    ContainerNode* node = (ContainerNode*) memory_aligned_alloc(sizeof(ContainerNode), alignof(ContainerNode));

    // Check if memory allocation was successful
    if (NULL == node) {
        LOG_ERROR("Failed to allocate memory to new ContainerNode.\n");
        return NULL; // Return NULL if allocation fails
    }

    // Initialize the new node's object and next pointer
    node->object = object;
    node->next = NULL;
    node->prev = NULL;
    return node;
}

void container_node_free(ContainerNode* node) {
    if (NULL == node) {
        return; // Nothing to free
    }

    // Assume the caller is responsible for freeing the object if necessary.
    free(node); // Free the node structure itself
}
