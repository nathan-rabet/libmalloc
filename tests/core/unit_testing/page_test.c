#include <criterion/criterion.h>

#include "page.h"

Test(page, floor_page)
{
    long page_size = PAGE_SIZE();

    cr_assert_eq(floor_to_page(0), 0);
    cr_assert_eq(floor_to_page(page_size - 1), 0);
    cr_assert_eq(floor_to_page(page_size), page_size);
    cr_assert_eq(floor_to_page(page_size + 1), page_size);
    cr_assert_eq(floor_to_page(page_size * 2 - 1), page_size);
    cr_assert_eq(floor_to_page(page_size * 2), page_size * 2);
}

Test(page, ceil_page)
{
    long page_size = PAGE_SIZE();

    cr_assert_eq(ceil_to_page(0), 0);
    cr_assert_eq(ceil_to_page(page_size - 1), page_size);
    cr_assert_eq(ceil_to_page(page_size), page_size);
    cr_assert_eq(ceil_to_page(page_size + 1), page_size * 2);
    cr_assert_eq(ceil_to_page(page_size * 2 - 1), page_size * 2);
    cr_assert_eq(ceil_to_page(page_size * 2), page_size * 2);
}

Test(page, begin_page)
{
    void *page_size = (void *)PAGE_SIZE();

    cr_assert_eq((void *)page_begin(NULL), NULL);
    cr_assert_eq((void *)page_begin(page_size - 1), (void *)0);
    cr_assert_eq((void *)page_begin(page_size), (void *)page_size);
    cr_assert_eq((void *)page_begin(page_size + 1), (void *)page_size);
    cr_assert_eq((void *)page_begin((void *)(2 * (long)page_size - 1)),
                 (void *)page_size);
    cr_assert_eq((void *)page_begin((void *)(2 * (long)page_size)),
                 (void *)(2 * (long)page_size));
}
