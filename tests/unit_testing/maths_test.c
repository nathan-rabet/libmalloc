#include <criterion/criterion.h>
#include <math.h>

#include "maths.h"

Test(log_2, log_2_of_1)

{
    // Matfh :)
    cr_assert_eq(log_2(0), 0, "log_2(0) = %lu", log_2(0));

    double x = 1.0;
    for (size_t i = 1; i < 100; i++)
    {
        size_t their = floor(log2(x));
        size_t our = log_2(i);

        cr_assert_eq(their, our,
                     "their_log(%lu) = %lu (float:%f), our_log(%lu) = %lu", i,
                     their, log2(x), i, our);
        x++;
    }
}
