/**
 * Copyright © 2023 Austin Berrio
 *
 * @file src/container/list.c
 */

#include "core/memory.h"
#include "core/logger.h"
#include "container/node.h"
#include "container/list.h"

#include <stdlib.h> // For malloc, free

ContainerList* container_list_create(void) {
    ContainerList* list
        = (ContainerList*) memory_alloc(sizeof(ContainerList), alignof(ContainerList));
    if (NULL == list) {
        LOG_ERROR("[ContainerList] Failed to allocate memory.");
        return NULL;
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

void container_list_free(ContainerList* list) {
    if (NULL == list) {
        return;
    }

    ContainerNode* current = list->head;
    while (current) {
        ContainerNode* next = current->next;
        container_node_free(current);
        current = next;
    }

    memory_free(list);
}

uint32_t container_list_size(const ContainerList* list) {
    return list->size;
}

bool container_list_is_empty(const ContainerList* list) {
    return 0 == list->size;
}

bool linked_list_append(ContainerList* list, void* data) {
    if (NULL == list) {
        LOG_ERROR("List is NULL.\n");
        return false;
    }

    // Create a new node
    ContainerNode* node = container_node_create(data);
    if (NULL == node) {
        LOG_ERROR("Failed to create node.\n");
        return false;
    }

    if (NULL == list->head) {
        // First node in the list
        list->head = node;
        list->tail = node;
    } else {
        // Append to tail
        node->prev = list->tail;
        list->tail->next = node;
        list->tail = node;
    }

    node->index = list->size; // Track the position of the node
    list->size++; // Increment the size of the list
    return true;
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

        node->next = current->next;
        current->next = node;
    }

    list->size++;
}

void linked_list_remove(ContainerList* list, void* object, linked_list_compare_t compare) {
    // Ensure the list is valid
    if (NULL == list || NULL == list->head) {
        LOG_ERROR("Invalid list or empty list.\n");
        return;
    }

    ContainerNode* current = list->head;
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
        current = current->next;
    }

    // If we reached here, the object was not found
    LOG_ERROR("Data not found in the list.\n");
}

ContainerNode*
linked_list_find(const ContainerList* list, const void* object, linked_list_compare_t compare);

void* linked_list_pop_last(ContainerList* list);

void* linked_list_pop_first(ContainerList* list);
