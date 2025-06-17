/**
 * Copyright © 2023 Austin Berrio
 *
 * @file src/container/node.c
 */

#include "core/logger.h"
#include "core/memory.h"
#include "container/node.h"

ContainerNode* container_node_create(void* data) {
    ContainerNode* node
        = (ContainerNode*) memory_alloc(sizeof(ContainerNode), alignof(ContainerNode));

    if (NULL == node) {
        LOG_ERROR("[ContainerNode] Failed to allocate memory.");
        return NULL;
    }

    node->index = SIZE_MAX; // SIZE_MAX is used to denote 'unset'
    node->data = data;
    node->next = NULL;
    node->prev = NULL;
    return node;
}

void container_node_free(ContainerNode* node) {
    if (NULL == node) {
        return;
    }
    memory_free(node);
}
