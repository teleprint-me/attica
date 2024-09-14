/**
 * Copyright © 2024 Austin Berrio
 *
 * @file src/prime.c
 */

#include "prime.h"

#include <stdbool.h>
#include <stddef.h>

prime_sample_t* prime_sample_create(uint32_t size) {
    if (2 > size) {
        LOG_ERROR("Prime number sample size must be greater than 2.\n");
        return NULL;
    }

    // Over-allocate by assuming every odd number is prime.
    // We can resize the array later.
    prime_sample_t* samples = calloc(1, sizeof(*samples));
    samples->data           = malloc(size * sizeof(int));

    uint32_t j = 0;

    // Handle the first prime (2) directly.
    samples->data[j++] = 2;

    // Generate primes starting from 3
    for (uint32_t i = 3; i <= size; i += 2) {
        bool is_prime = true;

        for (uint32_t divisor = 3; divisor * divisor <= i; divisor += 2) {
            if (i % divisor == 0) {
                is_prime = false;
                break;
            }
        }

        if (is_prime) {
            samples->data[j++] = i;
        }
    }

    // Resize array to actual number of primes found
    samples->data = realloc(samples->data, j * sizeof(int));
    samples->size = j;

    return samples;
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
