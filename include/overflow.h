#ifndef OVERFLOW_H
#define OVERFLOW_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Detects overflow in ptr + (nmemb * size)
 *
 * @param ptr The base pointer
 * @param nmemb The number of elements
 * @param size The size of each element
 * @return true if an overflow occurs, false otherwise
 */
bool ptr_addition_has_overflow(void *ptr, size_t nmemb, size_t size);

#endif /* OVERFLOW_H */
