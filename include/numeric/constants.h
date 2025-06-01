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

/** Mathematical Constants */

#ifndef M_PI
    #define M_PI 3.14159265358979323846f /* Circumference / diameter */
#endif

#ifndef PI
    #define PI M_PI
#endif

#ifndef SQRT_2
    #define SQRT_2 1.41421356237309504880f /* sqrt(2) */
#endif

#ifndef SQRT_2_PI
    #define SQRT_2_PI 0.79788456080286535588f /* sqrt(2/pi) */
#endif

/** Floating Point Epsilon */

#ifndef DOUBLE_EPSILON
    #define DOUBLE_EPSILON 1e-15
#endif

#ifndef SINGLE_EPSILON
    #define SINGLE_EPSILON 1e-7f
#endif

/** Type-Safe, Single-Eval Math Macros (GNU/Clang only) */

/**
 * @note MIN/MAX/CLAMP use GNU C extensions:
 *   - __typeof__ : Type deduction
 *   - Statement expressions ({ ... })
 * These macros are not standard C or MSVC compatible!
 */
#if defined(__GNUC__) || defined(__clang__)
    #ifndef MIN
        #define MIN(a, b) \
            ({ \
                __typeof__(a) _a = (a); \
                __typeof__(b) _b = (b); \
                _a < _b ? _a : _b; \
            })
    #endif
    #ifndef MAX
        #define MAX(a, b) \
            ({ \
                __typeof__(a) _a = (a); \
                __typeof__(b) _b = (b); \
                _a > _b ? _a : _b; \
            })
    #endif
#else
    /* Fallback: Classic C macros (may double-evaluate!) */
    #ifndef MIN
        #define MIN(a, b) (((a) < (b)) ? (a) : (b))
    #endif
    #ifndef MAX
        #define MAX(a, b) (((a) > (b)) ? (a) : (b))
    #endif

    /* Or: #warning "MIN, MAX, CLAMP macros use double-evaluation. Use with care!" */
#endif

#ifndef CLAMP
    #define CLAMP(x, lo, hi) (MAX((lo), MIN((x), (hi))))
#endif

#endif /* NUMERIC_CONSTANTS_H */
