#ifndef CAST_H
#define CAST_H

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Because of the EPITA coding style, we can't use the C cast.
 *
 * @param ptr The pointer to cast
 * @return void* The casted pointer
 */
void *cast_ptr(void *ptr);

/**
 * @brief Convert a pointer to a size_t. (EPITA coding style)
 *
 * @param ptr The pointer to convert
 * @return size_t The converted size_t
 */
size_t cast_ptr_to_size_t(void *ptr);

/**
 * @brief Cast a 64 bits variale to a 32 bits one.
 *
 * @param a The 64 bits variable to cast
 * @return uint32_t The 32 bits variable
 */
uint32_t cast_reduce_64bits_to_32bits(uint64_t a);

#endif /* CAST_H */
