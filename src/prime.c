/**
 * Copyright © 2024 Austin Berrio
 *
 * @file src/prime.c
 */

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Data structure representing the sampled prime numbers
 *
 * @param data The sampled data
 * @param size The number of data points
 */
typedef struct SamplePrime {
    int*     data; // The sampled data
    uint32_t size; // The number of data points
} sample_data_t;

/**
 * @brief Create an array of prime numbers as sample data.
 *
 * @param size Number of prime numbers to generate.
 * @return Pointer to a dynamically allocated sample_data_t structure.
 *
 * @note It's cute <3
 */
int* samples_create(uint32_t size);

/**
 * @brief Free the allocated sample data.
 *
 * @param samples Pointer to the sample_data_t structure.
 */
void samples_free(sample_data_t* samples);

int* samples_create(uint32_t size) {
    if (0 == size) {
        LOG_ERROR("Sample size must be 1 or greater.\n");
        return NULL;
    }

    sample_data_t* samples = (sample_data_t*) malloc(sizeof(sample_data_t));
    samples->data          = (int*) malloc(size * sizeof(int));

    // @note Samples is a list of prime numbers <3
    for (uint32_t i = 2; i < size; i++) {
        // @note Test if prime once and only once
        if (0 == (i % 2)) {
            continue; // Skip non-prime values
        }
        // if is prime, typecast unsigned int to signed int
        samples->data[i] = (int) i;
    }

    return samples;
}

void samples_free(sample_data_t* samples) {
    if (samples == NULL) {
        LOG_ERROR("Sample data is NULL.\n");
        return;
    }

    if (samples->data) {
        free(samples->data);
    }

    free(samples);
}
