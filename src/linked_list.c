/**
 * Copyright © 2024 Austin Berrio
 *
 * @file src/linked_list.c
 */

#include "linked_list.h"

linked_list_t* linked_list_create(void);

void linked_list_free(linked_list_t* list);

void linked_list_append(linked_list_t* list, void* data);

void linked_list_prepend(linked_list_t* list, void* data);

void linked_list_insert(linked_list_t* list, uint32_t index, void* data);

void linked_list_remove(linked_list_t* list, void* data);

uint32_t linked_list_size(const linked_list_t* list);

bool linked_list_is_empty(const linked_list_t* list);

node_t* linked_list_find(
    const linked_list_t* list, const void* data, linked_list_compare_t compare
);

void* linked_list_pop_last(linked_list_t* list);

void* linked_list_pop_first(linked_list_t* list);
