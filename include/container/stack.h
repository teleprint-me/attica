/**
 * Copyright © 2023 Austin Berrio
 *
 * @file include/container/stack.h
 */

#ifndef CONTAINER_STACK_H
#define CONTAINER_STACK_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "container/node.h"

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct ContainerStack {
    uint32_t size;
    ContainerNode* node;
} ContainerStack;

ContainerStack* container_stack_create(void);
void container_stack_free(ContainerStack* stack);

bool  container_stack_push(ContainerStack* stack, void* object);
void* container_stack_pop(ContainerStack* stack);
void* container_stack_peek(const ContainerStack* stack);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CONTAINER_STACK_H
