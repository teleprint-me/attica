/**
 * Copyright © 2024 Austin Berrio
 *
 * @file include/linked_list.h
 */

#include "node.h"

#include <stdint.h>

typedef struct LinkedList {
    Node*    head; // Pointer to the first node in the list
    uint32_t size; // Keeps track of the number of elements
} LinkedList;

LinkedList* linked_list_create();
void        linked_list_free(LinkedList* list);

void linked_list_append(LinkedList* list, void* data);
void linked_list_prepend(LinkedList* list, void* data);
void linked_list_remove(LinkedList* list, void* data);
