#ifndef BIT_H
#define BIT_H

#include <stddef.h>

typedef unsigned char byte_t; // Byte type
#define BYTES_TO_BITS(NB_BYTES) ((NB_BYTES)*8)
#define BITS_TO_BYTES(NB_BITS) ((NB_BITS) / 8)

/**
 * @brief Get the one bit at the given position.
 *
 * @param buffer The buffer to get the bit from.
 * @param bit_index The index of the bit to get.
 * @return byte_t The bit at the given position as a byte_t,
 * 0xFF if buffer is NULL.
 */
byte_t get_bit(byte_t *buffer, size_t bit_index);

/**
 * @brief Set the one bit at the given position.
 *
 * @param buffer The buffer to set the bit to.
 * @param value The value to set the bit to (0 or 1).
 * @param bit_index The index of the bit to set.
 */
void set_bit(byte_t *buffer, byte_t value, size_t bit_index);

#endif /* BIT_H */
