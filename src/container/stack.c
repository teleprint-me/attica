/**
 * Copyright © 2023 Austin Berrio
 *
 * @file src/container/stack.c
 */

#include "core/memory.h"
#include "core/logger.h"

#include "container/stack.h"

ContainerStack* container_stack_create(void) {
    ContainerStack* stack = memory_alloc(sizeof(ContainerStack), alignof(ContainerStack));
    if (!stack) {
        LOG_ERROR("Failed to create stack container.");
        return NULL;
    }
    stack->node = NULL;
    stack->size = 0;
    return stack;
}

void container_stack_free(ContainerStack* stack) {
    if (stack) {
        ContainerNode* current = stack->node;
        while (current) {
            ContainerNode* next = current->next;
            container_node_free(current);
            current = next;
        }
        memory_free(stack);
    }
}

bool container_stack_push(ContainerStack* stack, void* object) {
    if (!stack || !object) {
        return false;
    }

    ContainerNode* node = container_node_create(object);
    if (!node) {
        return false;
    }

    node->next = stack->node;
    if (stack->node) {
        stack->node->prev = node;
    }

    stack->node = node;
    stack->size += 1;
    return true;
}

void* container_stack_pop(ContainerStack* stack) {
    if (!stack || stack->size == 0) {
        LOG_ERROR("Stack is empty or NULL.");
        return NULL;
    }

    ContainerNode* current = stack->node;
    if (!current) {
        LOG_ERROR("Stack node is unexpectedly NULL.");
        return NULL;
    }

    void* object = current->object;
    stack->node = current->next;
    if (stack->node) {
        stack->node->prev = NULL;
    }

    container_node_free(current);
    stack->size -= 1;
    return object;
}

void* container_stack_peek(const ContainerStack* stack) {
    if (!stack || !stack->node || stack->size == 0) {
        LOG_ERROR("Cannot peek: stack is NULL or empty.");
        return NULL;
    }

    return stack->node->object;
}
