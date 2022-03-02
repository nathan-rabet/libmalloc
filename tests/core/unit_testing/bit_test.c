#include <criterion/criterion.h>

#include "bit.h"

Test(get_bit, null_safety)
{
    cr_assert_eq(get_bit(NULL, 0), 0xFF);
}

Test(get_bit, get_bit_all_combinations)
{
    byte_t buffer = 0; // 0b0

    for (size_t i = 0; i < 8; i++)
        cr_assert_eq(get_bit(&buffer, i), 0, "get_bit(0b0, %zu) != 0", i);
}

Test(get_bit, get_bit_0b1)
{
    byte_t buffer = 1; // 0b00000001

    for (size_t i = 0; i < 8; i++)
        cr_assert_eq(get_bit(&buffer, i), i == 7, "get_bit(0b1, %zu) != 0",
                     i == 7);
}

Test(get_bit, get_bit_0b101_0)
{
    byte_t buffer = 0xa0; // 0b10100000;

    cr_assert_eq(get_bit(&buffer, 0), 1);
    cr_assert_eq(get_bit(&buffer, 1), 0);
    cr_assert_eq(get_bit(&buffer, 2), 1);
    for (size_t i = 3; i < 8; i++)
        cr_assert_eq(get_bit(&buffer, i), 0, "get_bit(0b101, %zu) != 0", i);
}

Test(get_bit, get_bit_buffer_0xFF_0b0)
{
    byte_t buffer[] = { 0xFF, 0 };

    for (size_t i = 0; i < 8; i++)
        cr_assert_eq(get_bit(buffer, i), 1, "get_bit(0xFF, %zu) != 1", i);
    for (size_t i = 8; i < 16; i++)
        cr_assert_eq(get_bit(buffer, i), 0, "get_bit(0xFF, %zu) != 0", i);
}

Test(get_bit, get_bit_buffer_0x00_0b1)
{
    byte_t buffer[] = { 0x00, 0x80 /*0b10000000*/ };

    for (size_t i = 0; i < 8; i++)
        cr_assert_eq(get_bit(buffer, i), 0, "get_bit(0x00, %zu) != 0", i);
    for (size_t i = 8; i < 16; i++)
        cr_assert_eq(get_bit(buffer, i), i == 8, "get_bit(0x00, %zu) != 1",
                     i == 8);
}

Test(get_bit, get_bit_BIG_buffer)
{
    // byte_t buffer[] = { 0b00010010, 0b00110100, 0b01010110,
    //                     0b01111000, 0b10011010, 0b10111100,
    //                     0b11011110, 0b11110000, 0b10000000 };

    byte_t buffer[] = { 18, 52, 86, 120, 154, 188, 222, 240, 128 };

    // First byte
    cr_assert_eq(get_bit(buffer, 0), 0);
    cr_assert_eq(get_bit(buffer, 1), 0);
    cr_assert_eq(get_bit(buffer, 2), 0);
    cr_assert_eq(get_bit(buffer, 3), 1);
    cr_assert_eq(get_bit(buffer, 4), 0);
    cr_assert_eq(get_bit(buffer, 5), 0);
    cr_assert_eq(get_bit(buffer, 6), 1);
    cr_assert_eq(get_bit(buffer, 7), 0);

    // Second byte
    cr_assert_eq(get_bit(buffer, 8), 0);
    cr_assert_eq(get_bit(buffer, 9), 0);
    cr_assert_eq(get_bit(buffer, 10), 1);
    cr_assert_eq(get_bit(buffer, 11), 1);
    cr_assert_eq(get_bit(buffer, 12), 0);
    cr_assert_eq(get_bit(buffer, 13), 1);
    cr_assert_eq(get_bit(buffer, 14), 0);
    cr_assert_eq(get_bit(buffer, 15), 0);

    // ...

    // Last byte
    cr_assert_eq(get_bit(buffer, 64), 1);
    cr_assert_eq(get_bit(buffer, 65), 0);
    cr_assert_eq(get_bit(buffer, 66), 0);
    cr_assert_eq(get_bit(buffer, 67), 0);
    cr_assert_eq(get_bit(buffer, 68), 0);
    cr_assert_eq(get_bit(buffer, 69), 0);
    cr_assert_eq(get_bit(buffer, 70), 0);
    cr_assert_eq(get_bit(buffer, 71), 0);
}

Test(set_bit, null_safety)
{
    // Must not crash
    set_bit(NULL, 0, 0);
}

Test(get_bit, set_invalid_value)
{
    byte_t buffer = 0;

    set_bit(&buffer, 69, 1);

    cr_assert_eq(buffer, 0);
}

Test(set_bit, set_0b0_first)
{
    byte_t buffer = 0;

    set_bit(&buffer, 1, 0);
    cr_assert_eq(buffer, 128, "set_bit(0b0, 1, 0) != 0b10000000");
}

Test(set_bit, unset_0b0_first)
{
    byte_t buffer = 0xFF;

    set_bit(&buffer, 0, 0);
    cr_assert_eq(buffer, 127, "set_bit(0xFF, 0, 1) != 0b01111111");
}

Test(set_bit, set_0b0_second)
{
    byte_t buffer = 0;

    set_bit(&buffer, 1, 1);
    cr_assert_eq(buffer, 64, "set_bit(0b0, 1, 1) != 0b01000000");
}

Test(set_bit, unset_0b0_second)
{
    byte_t buffer = 0xFF;

    set_bit(&buffer, 0, 1);
    cr_assert_eq(buffer, 0xbf, "set_bit(0xFF, 0, 1) != 0b10111111, but 0b%x",
                 buffer);
}

Test(set_bit, set_0b0_third)
{
    byte_t buffer = 0;

    set_bit(&buffer, 1, 2);
    cr_assert_eq(buffer, 32,
                 "set_bit(0b0, 1, 2) != 0b00100000, but "
                 "0x%08X",
                 buffer);
}

Test(set_bit, unset_0b0_third)
{
    byte_t buffer = 0xFF;

    set_bit(&buffer, 0, 2);
    cr_assert_eq(buffer, 0xdf, "set_bit(0xFF, 0, 2) != 0b11011111, but 0x%X",
                 buffer);
}

Test(set_bit, set_0b0_last)
{
    byte_t buffer = 0;

    set_bit(&buffer, 1, 7);
    cr_assert_eq(buffer, 1,
                 "set_bit(0b0, 1, 7) != 0b00000001, "
                 "but 0x%X",
                 buffer);
}

Test(set_bit, unset_0b0_last)
{
    byte_t buffer = 0xFF;

    set_bit(&buffer, 0, 7);
    cr_assert_eq(buffer, 254, "set_bit(0b0, 0, 7) != 0b11111110");
}

Test(set_bit, set_buffer_0xFF_first)
{
    byte_t buffer[] = { 255, 0 };

    set_bit(buffer, 0, 0);
    cr_assert_eq(buffer[0], 127, "set_bit(0xFF, 0) != 0b01111111");
    cr_assert_eq(buffer[1], 0, "set_bit(0xFF, 0) != 0b0");
}

// Macro test mdr
Test(bit_macro, bytes_to_bits_conversion)
{
    cr_assert_eq(BYTES_TO_BITS(0), 0, "BYTES_TO_BITS(1) != 0");
    cr_assert_eq(BYTES_TO_BITS(1), 8, "BYTES_TO_BITS(1) != 8");
    cr_assert_eq(BYTES_TO_BITS(2), 16, "BYTES_TO_BITS(2) != 16");
    cr_assert_eq(BYTES_TO_BITS(3), 24, "BYTES_TO_BITS(3) != 24");
    cr_assert_eq(BYTES_TO_BITS(4), 32, "BYTES_TO_BITS(4) != 32");
}

Test(bit_macro, bits_to_bytes_conversion)
{
    cr_assert_eq(BITS_TO_BYTES(0), 0, "BITS_TO_BYTES(0) != 0");
    cr_assert_eq(BITS_TO_BYTES(8), 1, "BITS_TO_BYTES(8) != 1");
    cr_assert_eq(BITS_TO_BYTES(16), 2, "BITS_TO_BYTES(16) != 2");
    cr_assert_eq(BITS_TO_BYTES(24), 3, "BITS_TO_BYTES(24) != 3");
    cr_assert_eq(BITS_TO_BYTES(32), 4, "BITS_TO_BYTES(32) != 4");
}
