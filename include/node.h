/**
 * Copyright © 2024 Austin Berrio
 *
 * @file include/node.h
 */

#ifndef DSA_NODE_H
#define DSA_NODE_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

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

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DSA_NODE_H
