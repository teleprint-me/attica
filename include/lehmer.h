/**
 * @file lehmer.h
 * @brief Thread-local Lehmer Random Number Generator
 *
 * Implements a 32-bit Lehmer RNG (Park-Miller LCG), using thread-local state.
 * Suitable for multi-threaded environments without locking overhead.
 *
 * References:
 * - https://dl.acm.org/doi/10.1145/63039.63042
 * - https://www.cs.wm.edu/~va/software/park/park.html
 */

#ifndef LEHMER_H
#define LEHMER_H

#include <stdint.h>

#if defined(__GNUC__) && !defined(_Thread_local)
    #define _Thread_local __thread
#endif

#ifndef thread_local
    #define thread_local _Thread_local
#endif

#define LEHMER_MODULUS 2147483647
#define LEHMER_MULTIPLIER 48271
#define LEHMER_SEED 123456789

typedef struct LehmerState {
    int64_t i;
    double o;
} LehmerState;

extern thread_local LehmerState lehmer_state;

void lehmer_initialize(int64_t seed);
int64_t lehmer_generate_int64(void);
int32_t lehmer_generate_int32(void);
double lehmer_generate_double(void);
float lehmer_generate_float(void);

#endif // LEHMER_H
