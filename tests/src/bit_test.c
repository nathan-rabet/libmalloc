#include <criterion/criterion.h>

#include "bit.h"

Test(get_bit, get_bit_0b0)
{
    byte_t buffer = 0b0;

    for (size_t i = 0; i < 8; i++)
        cr_assert_eq(get_bit(buffer, i), 0, "get_bit(0b0, %zu) != 0", i);
}

Test(get_bit, get_bit_0b1)
{
    byte_t buffer = 0b1;

    cr_assert_eq(get_bit_buffer(&buffer, 0), 1);
    for (size_t i = 1; i < 8; i++)
        cr_assert_eq(get_bit(buffer, i), 0, "get_bit(0b1, %zu) != 0", i);
}

Test(get_bit, get_bit_0b101_0)
{
    byte_t buffer = 0b101;

    cr_assert_eq(get_bit_buffer(&buffer, 0), 1);
    cr_assert_eq(get_bit_buffer(&buffer, 1), 0);
    cr_assert_eq(get_bit_buffer(&buffer, 2), 1);
    for (size_t i = 3; i < 8; i++)
        cr_assert_eq(get_bit(buffer, i), 0, "get_bit(0b101, %zu) != 0", i);
}

Test(get_bit, get_bit_buffer_0xFF_0b0)
{
    byte_t buffer[] = { 0xFF, 0b0 };

    for (size_t i = 0; i < 8; i++)
        cr_assert_eq(get_bit_buffer(buffer, i), 1,
                     "get_bit_buffer(0xFF, %zu) != 1", i);
    for (size_t i = 8; i < 16; i++)
        cr_assert_eq(get_bit_buffer(buffer, i), 0,
                     "get_bit_buffer(0xFF, %zu) != 0", i);
}

Test(get_bit, get_bit_buffer_0x00_0b1)
{
    byte_t buffer[] = { 0x00, 0b1 };

    for (size_t i = 0; i < 8; i++)
        cr_assert_eq(get_bit_buffer(buffer, i), 0,
                     "get_bit_buffer(0x00, %zu) != 0", i);
    for (size_t i = 8; i < 16; i++)
        cr_assert_eq(get_bit_buffer(buffer, i), 1,
                     "get_bit_buffer(0x00, %zu) != 1", i);
}

Test(get_bit, get_bit_BIG_buffer)
{
    byte_t buffer[] = { 0b00010010, 0b00110100, 0b01010110,
                        0b01111000, 0b10011010, 0b10111100,
                        0b11011110, 0b11110000, 0b1 };

    // First byte
    cr_assert_eq(get_bit_buffer(buffer, 0), 0);
    cr_assert_eq(get_bit_buffer(buffer, 1), 0);
    cr_assert_eq(get_bit_buffer(buffer, 2), 0);
    cr_assert_eq(get_bit_buffer(buffer, 3), 1);
    cr_assert_eq(get_bit_buffer(buffer, 4), 0);
    cr_assert_eq(get_bit_buffer(buffer, 5), 0);
    cr_assert_eq(get_bit_buffer(buffer, 6), 1);
    cr_assert_eq(get_bit_buffer(buffer, 7), 0);

    // Second byte
    cr_assert_eq(get_bit_buffer(buffer, 8), 0);
    cr_assert_eq(get_bit_buffer(buffer, 9), 0);
    cr_assert_eq(get_bit_buffer(buffer, 10), 1);
    cr_assert_eq(get_bit_buffer(buffer, 11), 1);
    cr_assert_eq(get_bit_buffer(buffer, 12), 0);
    cr_assert_eq(get_bit_buffer(buffer, 13), 1);
    cr_assert_eq(get_bit_buffer(buffer, 14), 0);
    cr_assert_eq(get_bit_buffer(buffer, 15), 0);

    // ...

    // Last byte
    cr_assert_eq(get_bit_buffer(buffer, 64), 1);
    cr_assert_eq(get_bit_buffer(buffer, 65), 0);
    cr_assert_eq(get_bit_buffer(buffer, 66), 0);
    cr_assert_eq(get_bit_buffer(buffer, 67), 0);
    cr_assert_eq(get_bit_buffer(buffer, 68), 0);
    cr_assert_eq(get_bit_buffer(buffer, 69), 0);
    cr_assert_eq(get_bit_buffer(buffer, 70), 0);
    cr_assert_eq(get_bit_buffer(buffer, 71), 0);
}

Test(set_bit, set_0b0_first)
{
    byte_t buffer = 0b0;

    set_bit(&buffer, 1, 0);
    cr_assert_eq(buffer, 0b10000000, "set_bit(0b0, 0) != 0b1");
}

Test(set_bit, unset_0b0_first)
{
    byte_t buffer = 0xFF;

    set_bit(&buffer, 0, 1);
    cr_assert_eq(buffer, 0b01111111, "set_bit(0b0, 0) != 0b1");
}

Test(set_bit, set_0b0_second)
{
    byte_t buffer = 0b0;

    set_bit(&buffer, 1, 1);
    cr_assert_eq(buffer, 0b01000000, "set_bit(0b0, 1) != 0b10");
}

Test(set_bit, unset_0b0_second)
{
    byte_t buffer = 0xFF;

    set_bit(&buffer, 0, 0);
    cr_assert_eq(buffer, 0b10111111, "set_bit(0b0, 1) != 0b10");
}

Test(set_bit, set_0b0_third)
{
    byte_t buffer = 0b0;

    set_bit(&buffer, 1, 2);
    cr_assert_eq(buffer, 0b00100000, "set_bit(0b0, 2) != 0b100");
}

Test(set_bit, unset_0b0_third)
{
    byte_t buffer = 0xFF;

    set_bit(&buffer, 0, 3);
    cr_assert_eq(buffer, 0b11011111, "set_bit(0b0, 2) != 0b100");
}

Test(set_bit, set_0b0_last)
{
    byte_t buffer = 0b0;

    set_bit(&buffer, 1, 7);
    cr_assert_eq(buffer, 0b00000001, "set_bit(0b0, 7) != 0b10000000");
}

Test(set_bit, unset_0b0_last)
{
    byte_t buffer = 0xFF;

    set_bit(&buffer, 0, 7);
    cr_assert_eq(buffer, 0b11111110, "set_bit(0b0, 7) != 0b10000000");
}

Test(set_bit, set_buffer_0xFF_first)
{
    byte_t buffer[] = { 0xFF, 0b0 };

    set_bit_buffer(buffer, 1, 0);
    cr_assert_eq(buffer[0], 0xFE, "set_bit_buffer(0xFF, 0) != 0xFE");
    cr_assert_eq(buffer[1], 0b0, "set_bit_buffer(0xFF, 0) != 0b0");
}

// Macro test mdr
Test(bit_macro, bits_to_byte_conversion)
{
    cr_assert_eq(GET_BITS_SIZE(0b0), 1, "GET_BITS_SIZE(0b0) != 1");
    cr_assert_eq(GET_BITS_SIZE(0b1), 1, "GET_BITS_SIZE(0b1) != 1");
    cr_assert_eq(GET_BITS_SIZE(0b10), 2, "GET_BITS_SIZE(0b10) != 2");
    cr_assert_eq(GET_BITS_SIZE(0b11), 2, "GET_BITS_SIZE(0b11) != 2");
    cr_assert_eq(GET_BITS_SIZE(0b100), 3, "GET_BITS_SIZE(0b100) != 3");
    cr_assert_eq(GET_BITS_SIZE(0b101), 3, "GET_BITS_SIZE(0b101) != 3");
    cr_assert_eq(GET_BITS_SIZE(0b110), 3, "GET_BITS_SIZE(0b110) != 3");
    cr_assert_eq(GET_BITS_SIZE(0b111), 3, "GET_BITS_SIZE(0b111) != 3");
    cr_assert_eq(GET_BITS_SIZE(0b1000), 4, "GET_BITS_SIZE(0b1000) != 4");
    cr_assert_eq(GET_BITS_SIZE(0b1001), 4, "GET_BITS_SIZE(0b1001) != 4");
    cr_assert_eq(GET_BITS_SIZE(0b1010), 4, "GET_BITS_SIZE(0b1010) != 4");
    cr_assert_eq(GET_BITS_SIZE(0b1011), 4, "GET_BITS_SIZE(0b1011) != 4");
    cr_assert_eq(GET_BITS_SIZE(0b1100), 4, "GET_BITS_SIZE(0b1100) != 4");
    cr_assert_eq(GET_BITS_SIZE(0b1101), 4, "GET_BITS_SIZE(0b1101) != 4");
    cr_assert_eq(GET_BITS_SIZE(0b1110), 4, "GET_BITS_SIZE(0b1110) != 4");
    cr_assert_eq(GET_BITS_SIZE(0b1111), 4, "GET_BITS_SIZE(0b1111) != 4");
    cr_assert_eq(GET_BITS_SIZE(0b10000), 5, "GET_BITS_SIZE(0b10000) != 5");
}
