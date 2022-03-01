#include <criterion/criterion.h>

#include "slab.h"

Test(slab_group, get_size)
{
    cr_assert_eq(get_group_size(0), 1);
    cr_assert_eq(get_group_size(1), 2);
    cr_assert_eq(get_group_size(2), 4);
    cr_assert_eq(get_group_size(3), 8);
    cr_assert_eq(get_group_size(4), 16);
    cr_assert_eq(get_group_size(5), 32);
}

Test(slab_group, group_create_check_zero)
{
    struct slab_group *group = slab_group_create(1, NULL);

    cr_assert_not_null(group);
    cr_assert_eq(group->size_multiplicity, 1);
    cr_assert_eq(group->cache.nb_cached_slabs, 0);
    for (size_t i = 0; i < 3; i++)
    {
        cr_assert_eq(group->cache.cached_slabs[i].free_bit_index, 0);
        cr_assert_eq(group->cache.cached_slabs[i].slab_group, 0);
        cr_assert_eq(group->cache.cached_slabs[i].free_bit_index, 0);
    }
    cr_assert_eq(group->next, NULL);
    cr_assert_eq(group->prev, NULL);
    cr_assert_eq(group->slabs_meta, NULL);

    slab_group_destroy_all(group);
}

Test(slab_group, group_create_increasing)
{
    struct slab_group *group = slab_group_create(0, NULL);
    group = slab_group_create(1, group);
    group = slab_group_create(2, group);
    group = slab_group_create(3, group);

    struct slab_group *parser = group;

    // Increasing order
    cr_assert_eq(parser->size_multiplicity, 0, "Size multiplicity is %d",
                 parser->size_multiplicity);
    parser = parser->next;
    cr_assert_eq(parser->size_multiplicity, 1);
    parser = parser->next;
    cr_assert_eq(parser->size_multiplicity, 2);
    parser = parser->next;
    cr_assert_eq(parser->size_multiplicity, 3);
    cr_assert_eq(parser->next, NULL);

    // Decreasing order
    parser = parser->prev;
    cr_assert_eq(parser->size_multiplicity, 2);
    parser = parser->prev;
    cr_assert_eq(parser->size_multiplicity, 1);
    parser = parser->prev;
    cr_assert_eq(parser->size_multiplicity, 0);
    cr_assert_eq(parser->prev, NULL);

    slab_group_destroy_all(group);
}

Test(slab_group, group_create_deacreasing)
{
    struct slab_group *group = slab_group_create(3, NULL);
    group = slab_group_create(2, group);
    group = slab_group_create(1, group);
    group = slab_group_create(0, group);

    struct slab_group *parser = group;

    // Increasing order
    cr_assert_eq(parser->size_multiplicity, 0);
    parser = parser->next;
    cr_assert_eq(parser->size_multiplicity, 1);
    parser = parser->next;
    cr_assert_eq(parser->size_multiplicity, 2);
    parser = parser->next;
    cr_assert_eq(parser->size_multiplicity, 3);
    cr_assert_eq(parser->next, NULL);

    // Decreasing order
    parser = parser->prev;
    cr_assert_eq(parser->size_multiplicity, 2);
    parser = parser->prev;
    cr_assert_eq(parser->size_multiplicity, 1);
    parser = parser->prev;
    cr_assert_eq(parser->size_multiplicity, 0);
    cr_assert_eq(parser->prev, NULL);

    slab_group_destroy_all(group);
}

Test(slab_group, group_create_duplicate)
{
    struct slab_group *group = slab_group_create(0, NULL);
    group = slab_group_create(1, group);
    group = slab_group_create(2, group);
    group = slab_group_create(2, group);
    group = slab_group_create(3, group);

    struct slab_group *parser = group;

    // Increasing order
    cr_assert_eq(parser->size_multiplicity, 0);
    parser = parser->next;
    cr_assert_eq(parser->size_multiplicity, 1);
    parser = parser->next;
    cr_assert_eq(parser->size_multiplicity, 2);
    parser = parser->next;
    cr_assert_eq(parser->size_multiplicity, 3);
    cr_assert_eq(parser->next, NULL);

    // Decreasing order
    parser = parser->prev;
    cr_assert_eq(parser->size_multiplicity, 2);
    parser = parser->prev;
    cr_assert_eq(parser->size_multiplicity, 1);
    parser = parser->prev;
    cr_assert_eq(parser->size_multiplicity, 0);
    cr_assert_eq(parser->prev, NULL);

    slab_group_destroy_all(group);
}

Test(slab_group, delete_first)
{
    struct slab_group *group = slab_group_create(0, NULL);
    group = slab_group_create(1, group);
    group = slab_group_create(2, group);
    group = slab_group_create(3, group);

    group = slab_group_delete(group);

    struct slab_group *parser = group;

    // Increasing order
    cr_assert_eq(parser->size_multiplicity, 1);
    parser = parser->next;
    cr_assert_eq(parser->size_multiplicity, 2);
    parser = parser->next;
    cr_assert_eq(parser->size_multiplicity, 3);
    cr_assert_eq(parser->next, NULL);

    // Decreasing order
    parser = parser->prev;
    cr_assert_eq(parser->size_multiplicity, 2);
    parser = parser->prev;
    cr_assert_eq(parser->size_multiplicity, 1);
    parser = parser->prev;
    cr_assert_eq(parser, NULL);
}

Test(slab_group, delete_middle)
{
    struct slab_group *group = slab_group_create(0, NULL);
    group = slab_group_create(1, group);
    group = slab_group_create(2, group);
    group = slab_group_create(3, group);

    // Delete group
    group->next->next = slab_group_delete(group->next->next);

    struct slab_group *parser = group;
    // Increasing order
    cr_assert_eq(parser->size_multiplicity, 0);
    parser = parser->next;
    cr_assert_eq(parser->size_multiplicity, 1);
    parser = parser->next;
    cr_assert_eq(parser->size_multiplicity, 3);
    cr_assert_eq(parser->next, NULL);

    // Decreasing order
    parser = parser->prev;
    cr_assert_eq(parser->size_multiplicity, 1);
    parser = parser->prev;
    cr_assert_eq(parser->size_multiplicity, 0);
    cr_assert_eq(parser->prev, NULL);

    slab_group_destroy_all(group);
}

Test(slab_group, delete_last)
{
    struct slab_group *group = slab_group_create(0, NULL);
    group = slab_group_create(1, group);
    group = slab_group_create(2, group);
    group = slab_group_create(3, group);

    group->next->next->next = slab_group_delete(group->next->next->next);

    struct slab_group *parser = group;

    // Increasing order
    cr_assert_eq(parser->size_multiplicity, 0);
    parser = parser->next;
    cr_assert_eq(parser->size_multiplicity, 1);
    parser = parser->next;
    cr_assert_eq(parser->size_multiplicity, 2);
    cr_assert_eq(parser->next, NULL);

    // Decreasing order
    parser = parser->prev;
    cr_assert_eq(parser->size_multiplicity, 1);
    parser = parser->prev;
    cr_assert_eq(parser->size_multiplicity, 0);
    cr_assert_eq(parser->prev, NULL);

    slab_group_destroy_all(group);
}