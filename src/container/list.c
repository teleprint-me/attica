/**
 * Copyright © 2023 Austin Berrio
 *
 * @file src/linked_list.c
 */

#include "core/logger.h"
#include "container/node.h"
#include "container/list.h"

#include <stdlib.h> // For malloc, free

int linked_list_numeric_compare(const void* a, const void* b) {
    // dereference numbers a and b
    int n_a = *(const int*) a;
    int n_b = *(const int*) b;
    // -1 if a < b, 0 if a == b, 1 if a > b.
    return (n_a > n_b) - (n_a < n_b);
}

ContainerList* linked_list_create(void) {
    // Allocate memory for the linked list
    ContainerList* list = (ContainerList*) malloc(sizeof(ContainerList));
    if (NULL == list) {
        LOG_ERROR("Failed to allocate memory to ContainerList\n");
        return NULL;
    }

    // Initialize the list properties
    list->head = NULL;
    list->size = 0;

    return list;
}

void linked_list_free(ContainerList* list, void (*callback)(void*)) {
    if (NULL == list) {
        return;
    }

    ContainerNode* current = list->head;
    while (NULL != current) {
        ContainerNode* next = current->next;
        if (callback) {
            callback(current->object);
        }
        memory_free(current);
        current = next;
    }

    memory_free(list);
}

void linked_list_append(ContainerList* list, void* object) {
    if (NULL == list) {
        LOG_ERROR("List is NULL.\n");
        return;
    }

    // Create a new node
    ContainerNode* node = container_node_create(object);
    if (NULL == node) {
        LOG_ERROR("Failed to create node.\n");
        return;
    }

    // If the list is empty, the new node becomes the head
    if (NULL == list->head) {
        list->head = node;
    } else {
        // Traverse to the end of the list
        ContainerNode* current = list->head;
        while (NULL != current->next) {
            current = current->next;
        }
        current->next = node; // Append the new node
    }

    // Increment the size of the list
    list->size++;
}

void linked_list_prepend(ContainerList* list, void* object) {
    if (NULL == list) {
        LOG_ERROR("List is NULL.\n");
        return;
    }

    // Create a new node
    ContainerNode* node = container_node_create(object);
    if (NULL == node) {
        LOG_ERROR("Failed to create node.\n");
        return;
    }

    // Insert the new node at the beginning
    node->next = list->head;
    list->head = node;

    // Increment the size of the list
    list->size++;
}

void linked_list_insert(ContainerList* list, void* object, uint32_t index) {
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
    ContainerNode* node = container_node_create(object);
    if (NULL == node) {
        LOG_ERROR("Failed to create node.\n");
        return;
    }

    if (0 == index) {
        // Insert at the beginning
        node->next = list->head;
        list->head = node;
    } else {
        ContainerNode* current = list->head;

        for (uint32_t i = 1; i < index && NULL != current->next; ++i) {
            current = current->next;
        }

        if (NULL == current) {
            LOG_ERROR("Invalid insertion position.\n");
            node_free(node);
            return;
        }

        node->next    = current->next;
        current->next = node;
    }

    list->size++;
}

void linked_list_remove(
    ContainerList* list, void* object, linked_list_compare_t compare
) {
    // Ensure the list is valid
    if (NULL == list || NULL == list->head) {
        LOG_ERROR("Invalid list or empty list.\n");
        return;
    }

    ContainerNode* current  = list->head;
    ContainerNode* previous = NULL;

    // Traverse the list to find the matching node
    while (NULL != current) {
        if (0 == compare(object, current->object)) {
            // Found the matching node to remove
            if (NULL == previous) {
                // Removing the head node
                list->head = current->next;
            } else {
                // Remove from the middle or end
                previous->next = current->next;
            }

            node_free(current); // Free the node's memory
            list->size--;

            LOG_INFO("Node removed successfully.\n");
            return;
        }

        // Move to the next node
        previous = current;
        current  = current->next;
    }

    // If we reached here, the object was not found
    LOG_ERROR("Data not found in the list.\n");
}

uint32_t linked_list_size(const ContainerList* list) {
    return list->size;
}

bool linked_list_is_empty(const ContainerList* list) {
    return 0 == list->size;
}

ContainerNode* linked_list_find(
    const ContainerList* list, const void* object, linked_list_compare_t compare
);

void* linked_list_pop_last(ContainerList* list);

void* linked_list_pop_first(ContainerList* list);
