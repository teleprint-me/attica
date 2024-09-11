/**
 * Copyright © 2024 Austin Berrio
 *
 * @file src/node.c
 */

#include "node.h"
#include "logger.h"

#include <stddef.h> // For NULL
#include <stdlib.h> // For malloc and free

Node* node_create(void* data) {
    // Allocate memory for the new node
    Node* new_node = (Node*) malloc(sizeof(Node));

    // Check if memory allocation was successful
    if (NULL == new_node) {
        LOG_ERROR("Failed to allocate memory to new Node.\n");
        return NULL; // Return NULL if allocation fails
    }

    // Initialize the new node's data and next pointer
    new_node->data = data;
    new_node->next = NULL;

    return new_node;
}

void node_free(Node* node) {
    if (NULL == node) {
        return; // Nothing to free
    }

    // Assume the caller is responsible for freeing the data if necessary.
    free(node); // Free the node structure itself
}
