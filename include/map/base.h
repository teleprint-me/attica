/**
 * @file include/map/base.h
 * @brief Shared interface for Hash Table implementations.
 */

#ifndef HASH_MAP_BASE_H
#define HASH_MAP_BASE_H

#include <stdint.h>
#include <pthread.h>

/**
 * @brief Possible outcomes for hash map operations.
 */
typedef enum HashMapState {
    HASH_MAP_STATE_SUCCESS, /**< Operation completed successfully. */
    HASH_MAP_STATE_ERROR, /**< General error occurred during operation. */
    HASH_MAP_STATE_KEY_EXISTS, /**< Duplicate key insertion attempted. */
    HASH_MAP_STATE_KEY_NOT_FOUND, /**< Key not found in the map. */
    HASH_MAP_STATE_FULL, /**< Hash map has reached maximum capacity. */
} HashMapState;

/**
 * @brief Types of keys supported by the hash map.
 */
typedef enum HashMapKeyType {
    HASH_MAP_KEY_TYPE_INTEGER, /**< Keys are integers (size_t). */
    HASH_MAP_KEY_TYPE_STRING, /**< Keys are null-terminated strings (uint8_t). */
    HASH_MAP_KEY_TYPE_ADDRESS /**< Keys are memory addresses (uintptr_t). */
} HashMapKeyType;

/**
 * @brief Represents a key-value pair entry in the hash map.
 */
typedef struct HashMapEntry {
    void* key; /**< Pointer to the key (type depends on HashMapKeyType). */
    void* value; /**< Pointer to the associated value. */
} HashMapEntry;

/**
 * @brief Core hash map structure.
 */
typedef struct HashMap {
    HashMapEntry* entries; /**< Array of hash entries. */
    HashMapKeyType key_type; /**< Type of keys stored. */
    size_t count; /**< Current number of entries in the map. */
    size_t size; /**< Total capacity of the hash map. */
    pthread_mutex_t thread_lock; /**< Mutex for thread safety. */

    size_t (*hash)(const void* key, size_t size, size_t i); /**< Hash probing function. */
    int (*compare)(const void* key1, const void* key2); /**< Key comparison function. */
} HashMap;

/**
 * @name Life-cycle Management
 * @{
 */

/**
 * @brief Creates a new hash map.
 *
 * @param initial_size Initial capacity of the map.
 * @param key_type Type of keys (integer, string, or address).
 * @return Pointer to the new hash map, or NULL on failure.
 */
HashMap* hash_map_create(size_t initial_size, HashMapKeyType key_type);

/**
 * @brief Frees a hash map and all associated memory.
 *
 * @param map Pointer to the hash map to free.
 */
void hash_map_free(HashMap* map);

/** @} */

#endif // HASH_MAP_BASH_H
