/**
 * @file include/lehmer.h
 * @ref https://dl.acm.org/doi/10.1145/63039.63042
 * @ref https://www.cs.wm.edu/~va/software/park/park.html
 */

#ifndef LEHMER_H
#define LEHMER_H

#include <stdint.h>

/**
 * @param LEHMER_MODULUS Mersenne prime number used as modulus (2^31 - 1).
 *
 * m: A large Mersenne prime number appropriate for a given bit width.
 *
 * @note Must be a mersenne prime number.
 * - 32-bit: 2^31 - 1 = 2147483647
 * - 64-bit: 2^61 - 1 = 2305843009213693951
 *
 * @note Originally chosen for the IBM System/360 to avoid overflow errors in
 * 32-bit integer arithmetic. This choice remains relevant for modern embedded
 * systems with similar constraints.
 */
#define LEHMER_MODULUS 2147483647

/**
 * @param LEHMER_MULTIPLIER Prime number used to scale the seed.
 *
 * a: A smaller prime number in the range 2, ..., m - 1
 *
 * @note Must be a prime number.
 *
 * @note The multiplier was specifically selected to prevent overflow errors on
 * hardware with 32-bit integer limitations. Despite being dated, this choice
 * is still effective in modern contexts where 32-bit constraints apply.
 *
 * @warning 16807 was originally chosen for safe 32-bit arithmetic, but is not
 * a prime number. Experimenting with 16763, 16787, or 16811 may be more
 * suitable considering they are prime numbers.
 */
#define LEHMER_MULTIPLIER 48271

/**
 * @param LEHMER_SEED Default seed value.
 *
 * z: A integer value in the range 1, ..., m - 1
 *
 * @note May be any value within the defined range.
 */
#define LEHMER_SEED 123456789

typedef struct LehmerSeed {
    int64_t i; // input seed value
    double o; // output norm value
} LehmerSeed;

extern LehmerSeed lehmer_seed;

/// @brief Initializes the random number generator
void lehmer_initialize(int64_t z);

/// @brief Generate the next step in the sequence
void lehmer_modulo(void);

/// @brief Normalizes a seed to a float in the range 0.0 to 1.0
void lehmer_normalize(void);

/// @brief Generates a random number
double lehmer_generate(void);

#endif // LEHMER_H
