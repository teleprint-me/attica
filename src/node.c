/**
 * Copyright © 2024 Austin Berrio
 *
 * @file src/node.c
 */

#include "node.h"
#include "logger.h"

#include <stddef.h> // For NULL
#include <stdlib.h> // For malloc and free

node_t* node_create(void* data) {
    // Allocate memory for the new node
    node_t* node = (node_t*) malloc(sizeof(node_t));

    // Check if memory allocation was successful
    if (NULL == node) {
        LOG_ERROR("Failed to allocate memory to new node_t.\n");
        return NULL; // Return NULL if allocation fails
    }

    // Initialize the new node's data and next pointer
    node->data = data;
    node->next = NULL;

    return node;
}

void node_free(node_t* node) {
    if (NULL == node) {
        return; // Nothing to free
    }

    // Assume the caller is responsible for freeing the data if necessary.
    free(node); // Free the node structure itself
}
