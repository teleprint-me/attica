
#ifndef M_PI
    #define M_PI 3.141592653589793f // circumference / diameter
#endif

#ifndef PI
    #define PI M_PI
#endif

#ifndef SQRT_2_PI
    #define SQRT_2_PI 0.7978845608028654f // sqrt(2 / pi)
#endif

#ifndef SQRT_2
    #define SQRT_2 1.4142135623730951f // sqrt(2)
#endif

/**
 * @brief The smallest difference between two distinct double-precision
 *        floating-point numbers (optional).
 *
 * @note For 64-bit doubles with a 53-bit significand, this is approximately
 *       10**-15.
 */
#ifndef DOUBLE_EPSILON
    #define DOUBLE_EPSILON 1e-15
#endif

/**
 * @brief The smallest difference between two distinct single-precision
 *        floating-point numbers (optional).
 *
 * @note For 32-bit floats with a 24-bit significand, this is approximately
 *       10**-7.
 */
#ifndef SINGLE_EPSILON
    #define SINGLE_EPSILON 1e-7
#endif

/**
 * @brief Get the minimum of two values.
 *
 * @note This macro compares two arguments and returns the smaller one.
 */
#define MIN(a, b)                  ((a) < (b) ? (a) : (b))

/**
 * @brief Get the maximum of two values.
 *
 * @note This macro compares two arguments and returns the larger one.
 */
#define MAX(a, b)                  ((a) > (b) ? (a) : (b))

/**
 * @brief Clamp a value to a specified range.
 *
 * @param value The input value to bind
 * @param lower The lower boundary
 * @param upper The upper boundary
 *
 * @note This macro ensures that a value stays within a given range by
 *       clamping it to the specified minimum and maximum values.
 */
#define CLAMP(value, lower, upper) (FIC_MAX(lower, FIC_MIN(value, upper)))
