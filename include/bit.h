#ifndef MEMPOOL_H
#define MEMPOOL_H

#include <stddef.h>

#define byte_t unsigned char // Byte type
#define GET_BITS_SIZE(NB_BYTES) ((NB_BYTES)*8)

/**
 * @brief Get the one bit at the given position.
 *
 * @param buffer The buffer to get the bit from.
 * @param bit_index The index of the bit to get.
 * @return byte_t
 */
byte_t get_bit_buffer(byte_t *buffer, int bit_index);

/**
 * @brief Set the one bit at the given position.
 *
 * @param buffer The buffer to set the bit to.
 * @param bit_index The index of the bit to set.
 * @return byte_t
 */
byte_t set_bit_buffer(byte_t *buffer, int bit_index);

#endif /* MEMPOOL_H */
