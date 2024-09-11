/**
 * Copyright © 2024 Austin Berrio
 *
 * @file include/node.h
 */

/**
 * @brief A point on a stem at which a leaf or leaves are inserted
 *
 * @param data Generic pointer to hold any type of data
 * @param next Pointer to the next node
 */
typedef struct Node {
    void*        data; // Generic pointer to hold any type of data
    struct Node* next; // Pointer to the next node
} Node;

Node* node_create(void* data);
void  node_free(Node* node);
