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

size_t container_list_size(const ContainerList* list) {
    return list->size;
}

bool container_list_is_empty(const ContainerList* list) {
    return 0 == list->size;
}

bool container_list_append(ContainerList* list, void* data) {
    if (NULL == list) {
        LOG_ERROR("[ContainerList] Attempted append on NULL list.");
        return false;
    }

    ContainerNode* node = container_node_create(data);
    if (NULL == node) {
        LOG_ERROR("[ContainerList] Failed to allocate new node.");
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

bool container_list_insert(ContainerList* list, void* data, size_t index) {
    if (NULL == list) {
        LOG_ERROR("[ContainerList] Attempted insert on NULL list.");
        return false;
    }

    if (index > list->size) {
        LOG_ERROR("[ContainerList] Invalid index %u (size is %zu).", index, list->size);
        return false;
    }

    ContainerNode* node = container_node_create(data);
    if (NULL == node) {
        LOG_ERROR("[ContainerList] Failed to allocate new node.");
        return false;
    }

    if (0 == index) {
        // Insert at the beginning
        node->next = list->head;
        if (NULL != list->head) {
            list->head->prev = node;
        } else { // List was empty
            list->tail = node; // Node is also the tail
        }
        list->head = node;
    } else if (index == list->size) {
        // Insert at the end - identical to append
        node->prev = list->tail;
        if (NULL != list->tail) {
            list->tail->next = node;
        }
        list->tail = node;
    } else {
        // Insert in the middle
        ContainerNode* current = list->head;
        for (size_t i = 0; i < index; i++) {
            current = current->next;
        }

        node->prev = current->prev;
        node->next = current;

        if (current->prev) {
            current->prev->next = node;
        }
        current->prev = node;

        if (1 == index) { // Edge case
            list->head->next = node; // Insert after head
        }
    }

    node->index = index;
    list->size++;
    return true;
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
