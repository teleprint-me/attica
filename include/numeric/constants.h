/**
 * Copyright © 2023 Austin Berrio
 * @file include/numeric/constants.h
 * @brief Common mathematical constants and generic math macros.
 *
 * @note This header uses GNU C extensions (__typeof__, statement expressions) for safe,
 * single-evaluation MIN/MAX. If you need ANSI portability, provide your own simpler macros.
 */

#ifndef NUMERIC_CONSTANTS_H
#define NUMERIC_CONSTANTS_H

#include <stddef.h> // for size_t
#include <stdint.h> // for fixed-width types

/** Math Constants */
#define PI 3.14159265358979323846
#define SQRT_2 1.41421356237309504880
#define SQRT_2_PI 0.79788456080286535588

/** Epsilon */
#define DOUBLE_EPSILON 1e-15
#define SINGLE_EPSILON 1e-7f

/** Inline type-safe functions */
static inline int min_int(int a, int b) { return a < b ? a : b; }
static inline unsigned int min_uint(unsigned int a, unsigned int b) { return a < b ? a : b; }
static inline long min_long(long a, long b) { return a < b ? a : b; }
static inline unsigned long min_ulong(unsigned long a, unsigned long b) { return a < b ? a : b; }
static inline float min_float(float a, float b) { return a < b ? a : b; }
static inline double min_double(double a, double b) { return a < b ? a : b; }

static inline int max_int(int a, int b) { return a > b ? a : b; }
static inline unsigned int max_uint(unsigned int a, unsigned int b) { return a > b ? a : b; }
static inline long max_long(long a, long b) { return a > b ? a : b; }
static inline unsigned long max_ulong(unsigned long a, unsigned long b) { return a > b ? a : b; }
static inline float max_float(float a, float b) { return a > b ? a : b; }
static inline double max_double(double a, double b) { return a > b ? a : b; }

/** Type-generic dispatch macros using ISO C11 _Generic */
#define MIN(a, b) \
    _Generic((a), \
        int: min_int, \
        unsigned int: min_uint, \
        long: min_long, \
        unsigned long: min_ulong, \
        float: min_float, \
        double: min_double, \
        default: min_ulong \
    )(a, b)

#define MAX(a, b) \
    _Generic((a), \
        int: max_int, \
        unsigned int: max_uint, \
        long: max_long, \
        unsigned long: max_ulong, \
        float: max_float, \
        double: max_double, \
        default: max_ulong \
    )(a, b)

#define CLAMP(x, lo, hi) MAX((lo), MIN((x), (hi)))

#endif /* NUMERIC_CONSTANTS_H */
