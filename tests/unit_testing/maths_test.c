#include <criterion/criterion.h>
#include <math.h>

#include "maths.h"

Test(log2ceil, power_of_2)
{
    // Matfh :)
    cr_assert_eq(log2ceil(0), 0, "log2ceil(0) = %lu", log2ceil(0));

    double x = 1.0;
    for (size_t i = 1; i <= 1024; i *= 2)
    {
        size_t their = ceil(log2(x));
        size_t our = log2ceil(i);

        cr_assert_eq(their, our,
                     "their_log(%lu) = %lu (float:%f), our_log(%lu) = %lu", i,
                     their, log2(x), i, our);
        x *= 2;
    }
}

Test(log2ceil, not_only_power_of_2)
{
    // Matfh :)
    cr_assert_eq(log2ceil(0), 0, "log2ceil(0) = %lu", log2ceil(0));

    double x = 1.0;
    for (size_t i = 1; i <= 1024; i++)
    {
        size_t their = ceil(log2(x));
        size_t our = log2ceil(i);

        cr_assert_eq(their, our,
                     "their_log(%lu) = %lu (float:%f), our_log(%lu) = %lu", i,
                     their, log2(x), i, our);
        x++;
    }
}
