/**
 * Copyright © 2024 Austin Berrio
 *
 * @file include/queue.h
 */

#include "linked_list.h"

/**
 * @brief A sequence of elements held in temporary storage awaiting processing
 */
typedef struct Queue {
    LinkedList* list; // Queue implemented as a linked list
} Queue;

/**
 * @brief Initialize a new queue.
 */
Queue* queue_create();

/**
 * @brief Free memory and destroy the queue.
 */
void queue_free(Queue* queue);

/**
 * @brief Add data to the end of the queue.
 */
void queue_enqueue(Queue* queue, void* data);

/**
 * @brief Remove data from the front of the queue.
 */
void* queue_dequeue(Queue* queue);

/**
 * @brief View the front element without removing it.
 */
void* queue_front(const Queue* queue);
