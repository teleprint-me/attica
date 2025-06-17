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

    list->size++;
    return true;
}

void* container_list_get_data(const ContainerList* list, size_t index) {
    if (!list || index >= list->size) {
        return NULL;
    }

    ContainerNode* current = list->head;
    for (size_t i = 0; i < index; i++) {
        current = current->next;
    }

    return current ? current->data : NULL;
}

size_t container_list_get_index(const ContainerList* list, const void* data) {
    if (!list || !data) {
        return SIZE_MAX; // sentinel not found
    }

    uintptr_t address = (uintptr_t) data;
    ContainerNode* current = list->head;

    for (size_t i = 0; current; i++) {
        if (address == (uintptr_t) current->data) {
            return i;
        }
        current = current->next;
    }

    return SIZE_MAX;
}

bool container_list_remove(ContainerList* list, void* data) {
    if (!list || !list->head || !data) {
        LOG_ERROR("[ContainerList] Invalid list or empty.");
        return false;
    }

    uintptr_t address = (uintptr_t) data;
    ContainerNode* current = list->head;

    while (current) {
        if (address == (uintptr_t) current->data) {
            // Relink neighbors
            if (current->prev) {
                current->prev->next = current->next;
            } else {
                // Node is head
                list->head = current->next;
            }

            if (current->next) {
                current->next->prev = current->prev;
            } else {
                // Node is tail
                list->tail = current->prev;
            }

            container_node_free(current);
            list->size--;

            LOG_INFO("[ContainerList] Node removed successfully.");
            return true;
        }

        current = current->next;
    }

    LOG_ERROR("[ContainerList] Data not found in the list.");
    return false;
}

void* container_list_pop(ContainerList* list) {
    if (!list || !list->tail) {
        return NULL;
    }

    ContainerNode* node = list->tail;
    void* data = node->data;

    if (node->prev) {
        node->prev->next = NULL;
        list->tail = node->prev;
    } else {
        // This was the only node in the list
        list->head = NULL;
        list->tail = NULL;
    }

    container_node_free(node);
    list->size--;

    return data;
}

void* container_list_pop_index(ContainerList* list, size_t index) {
    if (!list || index >= list->size) {
        return NULL;
    }

    ContainerNode* current = list->head;
    for (size_t i = 0; i < index; i++) {
        current = current->next;
    }

    void* data = current->data;

    if (current->prev) {
        current->prev->next = current->next;
    } else {
        // Removing head
        list->head = current->next;
    }

    if (current->next) {
        current->next->prev = current->prev;
    } else {
        // Removing tail
        list->tail = current->prev;
    }

    container_node_free(current);
    list->size--;

    return data;
}
