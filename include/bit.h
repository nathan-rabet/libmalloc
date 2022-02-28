#ifndef BIT_H
#define BIT_H

#include <stddef.h>

#define byte_t unsigned char // Byte type
#define GET_BITS_SIZE(NB_BYTES) ((NB_BYTES)*8)

/**
 * @brief Get the one bit at the given position.
 *
 * @param buffer The buffer to get the bit from.
 * @param bit_index The index of the bit to get.
 * @return byte_t The bit at the given position as a byte_t.
 */
byte_t get_bit_buffer(byte_t *buffer, int bit_index);

/**
 * @brief Set the one bit at the given position.
 *
 * @param buffer The buffer to set the bit to.
 * @param value The value to set the bit to (0 or 1).
 * @param bit_index The index of the bit to set.
 */
void set_bit_buffer(byte_t *buffer, byte_t value, int bit_index);

#endif /* BIT_H */
