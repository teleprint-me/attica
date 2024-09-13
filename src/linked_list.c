/**
 * Copyright © 2024 Austin Berrio
 *
 * @file src/linked_list.c
 */

#include "linked_list.h"
#include "logger.h"

#include <stdlib.h> // For malloc, free

linked_list_t* linked_list_create(void) {
    // Allocate memory for the linked list
    linked_list_t* list = (linked_list_t*) malloc(sizeof(linked_list_t));
    if (NULL == list) {
        LOG_ERROR("Failed to allocate memory to linked_list_t\n");
        return NULL;
    }

    // Initialize the list properties
    list->head = NULL;
    list->size = 0;

    return list;
}

void linked_list_free(linked_list_t* list, void (*callback)(void*)) {
    if (NULL == list) {
        return;
    }

    node_t* current = list->head;
    while (NULL != current) {
        node_t* next = current->next;
        if (callback) {
            callback(current->data);
        }
        free(current);
        current = next;
    }

    free(list);
}

void linked_list_append(linked_list_t* list, void* data) {
    if (NULL == list) {
        LOG_ERROR("List is NULL.\n");
        return;
    }

    // Create a new node
    node_t* node = node_create(data);
    if (NULL == node) {
        LOG_ERROR("Failed to create node.\n");
        return;
    }

    // If the list is empty, the new node becomes the head
    if (NULL == list->head) {
        list->head = node;
    } else {
        // Traverse to the end of the list
        node_t* current = list->head;
        while (NULL != current->next) {
            current = current->next;
        }
        current->next = node; // Append the new node
    }

    // Increment the size of the list
    list->size++;
}

void linked_list_prepend(linked_list_t* list, void* data) {
    if (NULL == list) {
        LOG_ERROR("List is NULL.\n");
        return;
    }

    // Create a new node
    node_t* new_head = node_create(data);
    if (NULL == new_head) {
        LOG_ERROR("Failed to create node.\n");
        return;
    }

    // Insert the new node at the beginning
    new_head->next = list->head;
    list->head     = new_head;

    // Increment the size of the list
    list->size++;
}

void linked_list_insert(linked_list_t* list, void* data, uint32_t index) {
    // Handle null list case
    if (NULL == list) {
        LOG_ERROR("List is NULL.\n");
        return;
    }

    // Check for invalid insertion index
    if (index > list->size) { // Allow inserting at the end
        LOG_ERROR("Invalid insertion position.\n");
        return;
    }

    // Create a new node
    node_t* node = node_create(data);
    if (NULL == node) {
        LOG_ERROR("Failed to create node.\n");
        return;
    }

    if (0 == index) {
        // Insert at the beginning
        node->next = list->head;
        list->head = node;
    } else {
        node_t* current = list->head;

        for (uint32_t i = 1; i < index && NULL != current->next; ++i) {
            current = current->next;
        }

        if (NULL == current) {
            LOG_ERROR("Invalid insertion position.\n");
            free(node);
            return;
        }

        node->next    = current->next;
        current->next = node;
    }

    list->size++;
}

void linked_list_remove(linked_list_t* list, void* data);

uint32_t linked_list_size(const linked_list_t* list);

bool linked_list_is_empty(const linked_list_t* list);

node_t* linked_list_find(
    const linked_list_t* list, const void* data, linked_list_compare_t compare
);

void* linked_list_pop_last(linked_list_t* list);

void* linked_list_pop_first(linked_list_t* list);
