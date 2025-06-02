/**
 * Copyright © 2023 Austin Berrio
 *
 * @file src/node.c
 */

#include "core/logger.h"
#include "core/memory.h"
#include "core/node.h"

#include <stddef.h> // For NULL
#include <stdlib.h> // For malloc and free

Node* node_create(void* data) {
    // Allocate memory for the new node
    Node* node = (Node*) memory_aligned_alloc(sizeof(Node), alignof(Node));

    // Check if memory allocation was successful
    if (NULL == node) {
        LOG_ERROR("Failed to allocate memory to new Node.\n");
        return NULL; // Return NULL if allocation fails
    }

    // Initialize the new node's data and next pointer
    node->data = data;
    node->next = NULL;

    return node;
}

void node_free(Node* node) {
    if (NULL == node) {
        return; // Nothing to free
    }

    // Assume the caller is responsible for freeing the data if necessary.
    free(node); // Free the node structure itself
}
