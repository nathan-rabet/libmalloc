#include "bit.h"

byte_t get_bit(byte_t *buffer, size_t bit_index)
{
    if (buffer == NULL)
        return 0xFF;
    return (buffer[bit_index / 8] >> (7 - bit_index % 8)) & 0x01;
}

void set_bit(byte_t *buffer, byte_t value, size_t bit_index)
{
    if (buffer != NULL && value < 2)
    {
        buffer[bit_index / 8] =
            (buffer[bit_index / 8] & ~(1 << (7 - bit_index % 8)))
            | (value << (7 - bit_index % 8));
    }
}
