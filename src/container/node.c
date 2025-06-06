/**
 * Copyright © 2023 Austin Berrio
 *
 * @file src/container/node.c
 */

#include "core/logger.h"
#include "core/memory.h"
#include "container/node.h"

ContainerNode* container_node_create(void* object) {
    ContainerNode* node
        = (ContainerNode*) memory_aligned_alloc(sizeof(ContainerNode), alignof(ContainerNode));

    if (NULL == node) {
        LOG_ERROR("Failed to allocate memory for ContainerNode.");
        return NULL;
    }

    node->index = SIZE_MAX; // SIZE_MAX is used to denote 'unset'
    node->object = object;
    node->next = NULL;
    node->prev = NULL;
    return node;
}

void container_node_free(ContainerNode* node) {
    if (NULL == node) {
        return;
    }
    memory_aligned_free(node);
}
