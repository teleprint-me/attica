/**
 * Copyright © 2023 Austin Berrio
 *
 * @file include/queue.h
 */

#ifndef DSA_CONTAINER_QUEUE_H
#define DSA_CONTAINER_QUEUE_H

#include "container/node.h"

/**
 * @brief A sequence of elements held in temporary storage awaiting processing
 */
typedef Container ContainerQueue;

/**
 * @brief Initialize a new queue.
 */
ContainerQueue* container_queue_create(void);

/**
 * @brief Free memory and destroy the queue.
 */
void container_queue_free(ContainerQueue* queue);

/**
 * @brief Add data to the end of the queue.
 */
void container_queue_enqueue(ContainerQueue* queue, void* data);

/**
 * @brief Remove data from the front of the queue.
 */
void* container_queue_dequeue(ContainerQueue* queue);

/**
 * @brief View the last enqueued element without removing it.
 */
void* container_queue_peek(const ContainerQueue* queue);

#endif // DSA_CONTAINER_QUEUE_H
