/**
 * @file src/lehmer.c
 */

#include "lehmer.h"

/**
 * Global State
 */

thread_local LehmerState lehmer_state = {
    .i = LEHMER_SEED,
    .o = 0.0,
};

/**
 * Private Functions
 */

static inline void lehmer_modulo(void) {
    const int64_t q = LEHMER_MODULUS / LEHMER_MULTIPLIER;
    const int64_t r = LEHMER_MODULUS % LEHMER_MULTIPLIER;

    int64_t hi = lehmer_state.i / q;
    int64_t lo = lehmer_state.i % q;

    int64_t t = LEHMER_MULTIPLIER * lo - r * hi;
    lehmer_state.i = (t > 0) ? t : t + LEHMER_MODULUS;
}

static inline void lehmer_normalize(void) {
    lehmer_state.o = (double) lehmer_state.i / (double) LEHMER_MODULUS;
}

/**
 * Public Functions
 */

void lehmer_initialize(int64_t seed) {
    lehmer_state.i = (seed > 0) ? seed : LEHMER_SEED;
}

int64_t lehmer_generate_int64(void) {
    lehmer_modulo();
    return lehmer_state.i;
}

int32_t lehmer_generate_int32(void) {
    return (int32_t) lehmer_generate_int64();
}

double lehmer_generate_double(void) {
    lehmer_modulo();
    lehmer_normalize();
    return lehmer_state.o;
}

float lehmer_generate_float(void) {
    return (float) lehmer_generate_double();
}
