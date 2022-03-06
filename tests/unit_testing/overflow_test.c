#include <criterion/criterion.h>

#include "overflow.h"

Test(ptr_addition_has_overflow, basic_overflow)
{
    void *ptr = (void *)__SIZE_MAX__;
    size_t nmemb = 1;
    size_t size = 1;

    cr_assert_eq(ptr_addition_has_overflow(ptr, nmemb, size), true);
}

Test(ptr_addition_has_overflow, mult_overflow)
{
    void *ptr = (void *)__SIZE_MAX__;
    size_t nmemb = __SIZE_MAX__ / 2;
    size_t size = 3;

    cr_assert_eq(ptr_addition_has_overflow(ptr, nmemb, size), true);
}

Test(ptr_addition_has_overflow, mult_overflow_2)
{
    void *ptr = (void *)__SIZE_MAX__;
    size_t nmemb = 3;
    size_t size = __SIZE_MAX__ / 2;

    cr_assert_eq(ptr_addition_has_overflow(ptr, nmemb, size), true);
}

Test(ptr_addition_has_overflow, no_overflow)
{
    void *ptr = (void *)0x1234567812345678;
    size_t nmemb = 24;
    size_t size = 42;

    cr_assert_eq(ptr_addition_has_overflow(ptr, nmemb, size), false);
}
