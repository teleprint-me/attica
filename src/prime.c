/**
 * Copyright © 2024 Austin Berrio
 *
 * @file src/prime.c
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Data structure representing the sampled prime numbers
 *
 * @param data The sampled data
 * @param size The number of data points
 */
typedef struct PrimeSample {
    int*     data; // The sampled data
    uint32_t size; // The number of data points
} prime_sample_t;

/**
 * @brief Create an array of prime numbers as sample data.
 *
 * @param size Number of prime numbers to generate.
 * @return Pointer to a dynamically allocated sample_data_t structure.
 *
 * @note It's cute <3
 */
int* prime_sample_create(uint32_t size);

/**
 * @brief Free the allocated sample data.
 *
 * @param samples Pointer to the sample_data_t structure.
 */
void prime_sample_free(prime_sample_t* samples);

int* prime_sample_create(uint32_t size) {
    if (0 == size) {
        return NULL;
    }

    prime_sample_t* samples = calloc(1, sizeof(*samples));

    // @note Samples is a list of prime numbers <3
    for (uint32_t i = 2, j = 0; i <= size && j < size; ++i) {
        bool is_prime = true;

        if (!(i & 1)) {
            continue; // Skip even numbers greater than 2
        }

        for (uint32_t divisor = 2; divisor * divisor <= i; ++divisor) {
            if (!(i % divisor)) {
                is_prime = false;
                break;
            }
        }

        // If the number is prime, assign its value to samples
        if (is_prime) {
            samples->data[j++] = i;
        }
    }

    samples->size = j;

    return samples->data;
}

void prime_sample_free(prime_sample_t* samples) {
    if (samples == NULL) {
        LOG_ERROR("Sample data is NULL.\n");
        return;
    }

    if (samples->data) {
        free(samples->data);
    }

    free(samples);
}
