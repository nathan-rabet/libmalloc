#ifndef CAST_H
#define CAST_H

#include <stddef.h>

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

#endif /* CAST_H */
