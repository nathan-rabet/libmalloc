#ifndef MATH_H
#define MATH_H

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
size_t power2(uint8_t n);

#endif /* MATH_H */
