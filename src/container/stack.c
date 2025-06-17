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
    stack->head = NULL;
    stack->tail = NULL; // Tail is unused in stack semantics.
    stack->size = 0;
    return stack;
}

void container_stack_free(ContainerStack* stack) {
    if (stack) {
        ContainerNode* current = stack->head;
        while (current) {
            ContainerNode* next = current->next;
            container_node_free(current);
            current = next;
        }
        memory_free(stack);
    }
}

bool container_stack_push(ContainerStack* stack, void* data) {
    if (!stack || !data) {
        return false;
    }

    ContainerNode* node = container_node_create(data);
    if (!node) {
        return false;
    }

    node->next = stack->head;
    if (stack->head) {
        stack->head->prev = node;
    }

    stack->head = node;
    stack->size += 1;
    return true;
}

void* container_stack_pop(ContainerStack* stack) {
    if (!stack || stack->size == 0) {
        LOG_ERROR("Stack is empty or NULL.");
        return NULL;
    }

    ContainerNode* current = stack->head;
    if (!current) {
        LOG_ERROR("Stack node is unexpectedly NULL.");
        return NULL;
    }

    void* object = current->data;
    stack->head = current->next;
    if (stack->head) {
        stack->head->prev = NULL;
    }

    container_node_free(current);
    stack->size -= 1;
    return object;
}

void* container_stack_peek(const ContainerStack* stack) {
    if (!stack || !stack->head || stack->size == 0) {
        LOG_ERROR("Cannot peek: stack is NULL or empty.");
        return NULL;
    }

    return stack->head->data;
}
