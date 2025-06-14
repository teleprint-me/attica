/**
 * Copyright © 2023 Austin Berrio
 *
 * @file src/hash_table.c
 * @brief Minimalistic hash table implementation providing mapping between integers, strings, and
 * memory addresses.
 *
 * The Hash Interface is designed to provide a minimal mapping between integers, strings, and memory
 * addresses, much like a dictionary in Python. Users can map integers, strings, and memory
 * addresses to other data types, supporting insertion, search, deletion, and table clearing.
 *
 * @note Comparison functions used with the HashTable must:
 * - Return 0 for equality.
 * - Return a non-zero value for inequality.
 *
 * @note Thread Safety:
 * - The hash table is designed to be thread-safe using mutexes. Ensure that all operations on the
 * hash table are performed within critical sections to avoid race conditions.
 *
 * @note Supported Keys:
 * - Integers (`uint64_t`)
 * - Strings (`char*`)
 * - Memory addresses (`uintptr_t`)
 *
 * @note Probing:
 * - Linear probing is used to handle collisions.
 */

#include "core/memory.h"

#include "core/logger.h"
#include "map/open-address.h"

#include <string.h>

/**
 * @section Hash Life-cycle
 */

HashTable* hash_table_create(uint64_t initial_size, HashTableType key_type) {
    HashTable* table = memory_alloc(sizeof(HashTable), alignof(HashTable));
    if (!table) {
        LOG_ERROR("Failed to allocate memory for HashTable.");
        return NULL;
    }

    table->count = 0;
    table->size = initial_size > 0 ? initial_size : 10;
    table->type = key_type;

    switch (table->type) {
        case HASH_TYPE_STRING:
            table->hash = hash_string;
            table->compare = hash_string_compare;
            break;
        case HASH_TYPE_INTEGER:
            table->hash = hash_integer;
            table->compare = hash_integer_compare;
            break;
        case HASH_TYPE_ADDRESS:
            table->hash = hash_address;
            table->compare = hash_address_compare;
            break;
        default:
            LOG_ERROR("Invalid HashTableType given.");
            memory_free(table);
            return NULL;
    }

    table->entries = memory_calloc(table->size, sizeof(HashTableEntry), alignof(HashTableEntry));
    if (!table->entries) {
        LOG_ERROR("Failed to allocate memory for HashTable entries.");
        memory_free(table);
        return NULL;
    }

    // Initialize the mutex for thread safety
    int error_code = pthread_mutex_init(&table->thread_lock, NULL);
    if (0 != error_code) {
        LOG_ERROR("Failed to initialize mutex with error: %d", error_code);
        memory_free(table->entries);
        memory_free(table);
        return NULL;
    }

    return table;
}

void hash_table_free(HashTable* table) {
    if (table) {
        // Destroy the mutex before freeing memory
        pthread_mutex_destroy(&table->thread_lock);

        if (table->entries) {
            memory_free(table->entries);
        }
        memory_free(table);
        table = NULL;
    }
}

/**
 * @section Private Functions
 */

static HashTableState hash_table_insert_internal(HashTable* table, const void* key, void* value) {
    if (!table || !table->entries || table->size == 0) {
        LOG_ERROR("Invalid table for insert internal.");
        return HASH_ERROR;
    }

    if (!key) {
        LOG_ERROR("Key is NULL.");
        return HASH_ERROR;
    }

    if (!value) {
        LOG_ERROR("Value is NULL.");
        return HASH_ERROR;
    }

    for (uint64_t i = 0; i < table->size; i++) {
        uint64_t index = table->hash(key, table->size, i);

        if (!table->entries[index].key) {
            table->entries[index].key = (void*) key;
            table->entries[index].value = value;
            table->count++;
            return HASH_SUCCESS;
        } else if (0 == table->compare(table->entries[index].key, key)) {
            return HASH_KEY_EXISTS;
        }
    }

    return HASH_TABLE_FULL;
}

static HashTableState hash_table_resize_internal(HashTable* table, uint64_t new_size) {
    if (!table || !table->entries || table->size == 0) {
        LOG_ERROR("Invalid table for resize internal.");
        return HASH_ERROR;
    }

    if (new_size <= table->size) {
        return HASH_SUCCESS;
    }

    HashTableEntry* new_entries = (HashTableEntry*) calloc(new_size, sizeof(HashTableEntry));
    if (!new_entries) {
        LOG_ERROR("Failed to allocate memory for resized table.");
        return HASH_ERROR;
    }

    // Backup
    HashTableEntry* old_entries = table->entries;
    uint64_t old_size = table->size;

    // Swap
    table->entries = new_entries;
    table->size = new_size;

    // Probe entries
    uint64_t rehashed_count = 0;
    for (uint64_t i = 0; i < old_size; i++) {
        HashTableEntry* entry = &old_entries[i];
        if (entry->key) {
            HashTableState state = hash_table_insert_internal(table, entry->key, entry->value);
            if (HASH_SUCCESS != state) {
                LOG_ERROR("Failed to rehash key during resize.");
                memory_free(new_entries);
                table->entries = old_entries;
                table->size = old_size;
                return state;
            }
            rehashed_count++;
        }
    }

    table->count = rehashed_count;
    memory_free(old_entries);
    return HASH_SUCCESS;
}

static HashTableState hash_table_delete_internal(HashTable* table, const void* key) {
    if (!table || !table->entries || table->size == 0) {
        LOG_ERROR("Invalid table for delete internal.");
        return HASH_ERROR;
    }

    if (!key) {
        LOG_ERROR("Key is NULL.");
        return HASH_ERROR;
    }

    for (uint64_t i = 0; i < table->size; i++) {
        uint64_t index = table->hash(key, table->size, i);
        HashTableEntry* entry = &table->entries[index];

        if (!entry->key) {
            return HASH_KEY_NOT_FOUND; // Stop probing
        }

        if (0 == table->compare(entry->key, key)) {
            // Delete entry
            entry->key = NULL;
            entry->value = NULL;
            table->count--;

            // Rehash the remainder of the probe sequence
            for (uint64_t j = i + 1; j < table->size; j++) {
                uint64_t rehash_index = table->hash(key, table->size, j);
                HashTableEntry* rehash_entry = &table->entries[rehash_index];

                if (!rehash_entry->key) {
                    break;
                }

                void* rehash_key = rehash_entry->key;
                void* rehash_value = rehash_entry->value;

                rehash_entry->key = NULL;
                rehash_entry->value = NULL;
                table->count--;

                // Reinsert into new position
                HashTableState state = hash_table_insert_internal(table, rehash_key, rehash_value);
                if (HASH_SUCCESS != state) {
                    LOG_ERROR("Failed to reinsert during delete.");
                    return HASH_ERROR;
                }
            }

            return HASH_SUCCESS;
        }
    }

    return HASH_KEY_NOT_FOUND;
}

static HashTableState hash_table_clear_internal(HashTable* table) {
    if (!table || !table->entries || table->size == 0) {
        LOG_ERROR("Invalid table for clear internal.");
        return HASH_ERROR;
    }

    for (uint64_t i = 0; i < table->size; i++) {
        table->entries[i].key = NULL;
        table->entries[i].value = NULL;
    }

    table->count = 0;
    return HASH_SUCCESS;
}

static void* hash_table_search_internal(HashTable* table, const void* key) {
    if (!table || !table->entries || table->size == 0) {
        LOG_ERROR("Invalid table for search internal.");
        return NULL;
    }

    if (!key) {
        LOG_ERROR("Key is NULL.");
        return NULL;
    }

    for (uint64_t i = 0; i < table->size; i++) {
        uint64_t index = table->hash(key, table->size, i);
        HashTableEntry* entry = &table->entries[index];

        if (!entry->key) {
            return NULL;
        }

        if (0 == table->compare(entry->key, key)) {
            return entry->value;
        }
    }

    return NULL;
}

/**
 * @section Hash Functions
 */

HashTableState hash_table_insert(HashTable* table, const void* key, void* value) {
    if (!table || !table->entries || table->size == 0) {
        LOG_ERROR("Invalid table for insert.");
        return HASH_ERROR;
    }

    if (!key) {
        LOG_ERROR("Key is NULL.");
        return HASH_ERROR;
    }

    if (!value) {
        LOG_ERROR("Value is NULL.");
        return HASH_ERROR;
    }

    HashTableState state;
    pthread_mutex_lock(&table->thread_lock);

    if ((double) table->count / table->size > 0.75) {
        if (HASH_SUCCESS != hash_table_resize_internal(table, table->size * 2)) {
            state = HASH_ERROR;
            goto exit;
        }
    }
    state = hash_table_insert_internal(table, key, value);

exit:
    pthread_mutex_unlock(&table->thread_lock);
    return state;
}

HashTableState hash_table_resize(HashTable* table, uint64_t new_size) {
    if (!table || !table->entries || table->size == 0) {
        LOG_ERROR("Invalid table for resize.");
        return HASH_ERROR;
    }

    HashTableState state;
    pthread_mutex_lock(&table->thread_lock);
    state = hash_table_resize_internal(table, new_size);
    pthread_mutex_unlock(&table->thread_lock);
    return state;
}

HashTableState hash_table_delete(HashTable* table, const void* key) {
    if (!table || !table->entries || table->size == 0) {
        LOG_ERROR("Invalid table for delete.");
        return HASH_ERROR;
    }

    if (!key) {
        LOG_ERROR("Key is NULL.");
        return HASH_ERROR;
    }

    HashTableState state;
    pthread_mutex_lock(&table->thread_lock);
    state = hash_table_delete_internal(table, key);
    pthread_mutex_unlock(&table->thread_lock);
    return state;
}

HashTableState hash_table_clear(HashTable* table) {
    if (!table || !table->entries || table->size == 0) {
        LOG_ERROR("Invalid table for clear.");
        return HASH_ERROR;
    }

    HashTableState state;
    pthread_mutex_lock(&table->thread_lock);
    state = hash_table_clear_internal(table);
    pthread_mutex_unlock(&table->thread_lock);
    return state;
}

void* hash_table_search(HashTable* table, const void* key) {
    if (!table || !table->entries || table->size == 0) {
        LOG_ERROR("Invalid table for search.");
        return NULL;
    }

    if (!key) {
        LOG_ERROR("Key is NULL.");
        return NULL;
    }

    void* value = NULL;
    pthread_mutex_lock(&table->thread_lock);
    value = hash_table_search_internal(table, key);
    pthread_mutex_unlock(&table->thread_lock);
    return value;
}

/**
 * @section Hash Integers
 */

uint64_t hash_integer(const void* key, uint64_t size, uint64_t i) {
    const int32_t* k = (int32_t*) key;
    uint64_t hash = *k * 2654435761U; // Knuth's multiplicative
    return (hash + i) % size;
}

int hash_integer_compare(const void* key1, const void* key2) {
    return *(const int32_t*) key1 - *(const int32_t*) key2;
}

int32_t* hash_integer_search(HashTable* table, const void* key) {
    return (int32_t*) hash_table_search_internal(table, key);
}

/**
 * @section Hash Strings
 */

uint64_t hash_djb2(const char* string) {
    uint64_t hash = 5381;
    int c;

    while ((c = *string++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }

    return hash;
}

uint64_t hash_string(const void* key, uint64_t size, uint64_t i) {
    const char* string = (const char*) key;
    return (hash_djb2(string) + i) % size;
}

int hash_string_compare(const void* key1, const void* key2) {
    return strcmp((const char*) key1, (const char*) key2);
}

char* hash_string_search(HashTable* table, const void* key) {
    return (char*) hash_table_search_internal(table, key);
}

/**
 * @section Hash Addresses
 */

uint64_t hash_address(const void* key, uint64_t size, uint64_t i) {
    uintptr_t addr = (uintptr_t) key;
    uint64_t hash = addr * 2654435761U; // Knuth's multiplicative
    return (hash + i) % size;
}

int hash_address_compare(const void* key1, const void* key2) {
    intptr_t a = (intptr_t) key1;
    intptr_t b = (intptr_t) key2;
    return (a > b) - (a < b);
}

void* hash_address_search(HashTable* table, const void* key) {
    return (void*) hash_table_search_internal(table, key);
}
