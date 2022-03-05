#ifndef MATHS_H
#define MATHS_H

#include <stddef.h>
#include <stdint.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

/**
 * @brief Return 2 ^ n.
 *
 * @param n The power to compute.
 * @return size_t The computed power.
 */
size_t power_2(uint8_t n);

/**
 * @brief Return the logarithm base 2 of a value.
 *
 * @param value The value to compute the logarithm of.
 * @return size_t The computed logarithm.
 */
size_t log_2(size_t value);

#endif /* MATHS_H */
