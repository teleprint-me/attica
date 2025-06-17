/**
 * Copyright © 2023 Austin Berrio
 *
 * @file include/container/stack.h
 */

#ifndef DSA_CONTAINER_STACK_H
#define DSA_CONTAINER_STACK_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "container/node.h"

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef Container ContainerStack;

ContainerStack* container_stack_create(void);
void container_stack_free(ContainerStack* stack);

bool  container_stack_push(ContainerStack* stack, void* data);
void* container_stack_pop(ContainerStack* stack);
void* container_stack_peek(const ContainerStack* stack);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DSA_CONTAINER_STACK_H
