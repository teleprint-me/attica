/**
 * Copyright © 2023 Austin Berrio
 *
 * @file include/float_is_close.h
 *
 * @brief Compare floating-point numbers with a given tolerance in pure C
 *
 * @note FIC is an abbreviation of FLOAT_IS_CLOSE
 *
 * @note see 1.2: Epsilon-Delta Definition of a Limit for details
 * https://math.libretexts.org/Bookshelves/Calculus/Calculus_3e_(Apex)
 */

#ifndef NUMERIC_FIC_H
#define NUMERIC_FIC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Determine if two double-precision floating-point numbers are close
 *        within a specified tolerance.
 *
 * @param a           The first floating-point number.
 * @param b           The second floating-point number.
 * @param significand The number of significant digits to consider (must be
 *                    in the range 1 to 15 inclusive). This determines the
 *                    absolute tolerance.
 *
 * @return            True if the numbers are close within the specified
 *                    tolerance, false otherwise.
 *
 * @note The significand is clamped if it is out of range.
 * @note FIC_DOUBLE_EPSILON affects relative tolerance.
 */
bool double_is_close(double a, double b, size_t significand);

/**
 * @brief Determine if two single-precision floating-point numbers are close
 *        within a specified tolerance.
 *
 * @param a           The first floating-point number.
 * @param b           The second floating-point number.
 * @param significand The number of significant digits to consider (must be
 *                    in the range 1 to 7 inclusive). This determines the
 *                    absolute tolerance.
 *
 * @return            True if the numbers are close within the specified
 *                    tolerance, false otherwise.
 *
 * @note The significand is clamped if it is out of range.
 * @note FIC_SINGLE_EPSILON affects relative tolerance.
 */
bool float_is_close(float a, float b, size_t significand);

#endif // NUMERIC_FIC_H
