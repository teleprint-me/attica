#include "lehmer.h"

/**
 * @section Private functions
 */

LehmerSeed lehmer_seed = {
    .i = LEHMER_SEED,
    .o = 0.0,
};

static inline void lehmer_modulo(void) {
    /// @important f(z) = (a * z) % m
    lehmer_seed.i = (LEHMER_MULTIPLIER * lehmer_seed.i) % LEHMER_MODULUS;
}

static inline void lehmer_normalize(void) {
    lehmer_seed.o = (double) lehmer_seed.i / (double) LEHMER_MODULUS;
}

/**
 * @section Initializer
 */

void lehmer_initialize(int64_t z) {
    lehmer_seed.i = (z > 0) ? z : LEHMER_SEED;
}

/**
 * @section Integer generators
 */

int64_t lehmer_generate_int64(void) {
    lehmer_modulo();
    return lehmer_seed.i;
}

int32_t lehmer_generate_int32(void) {
    return (int32_t) lehmer_generate_int64();
}

/**
 * @section Float generators
 */

double lehmer_generate_double(void) {
    lehmer_modulo();
    lehmer_normalize();
    return lehmer_seed.o;
}

float lehmer_generate_float(void) {
    return (float) lehmer_generate_double();
}
