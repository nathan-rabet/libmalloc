#include <criterion/criterion.h>

#include "maths.h"
#include "slab.h"

Test(slab_group_power, power_of_2)
{
    cr_assert_eq(power2(0), 1);
    cr_assert_eq(power2(1), 2);
    cr_assert_eq(power2(2), 4);
    cr_assert_eq(power2(3), 8);
    cr_assert_eq(power2(4), 16);
    cr_assert_eq(power2(5), 32);
}

Test(slab_group_create, group_create_check_meta)
{
    struct slab_group *group = slab_group_create(0, NULL);

    cr_assert_not_null(group);
    cr_assert_eq(group->size_multiplicity, 0);
    cr_assert_not_null(group->slabs_meta);
    cr_assert_eq(power2(group->slabs_meta->common_group->size_multiplicity), 1);

    slab_group_destroy_all(group);
}

Test(slab_group_create, group_create_check_zero)
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

    slab_group_destroy_all(group);
}

Test(slab_group_create, group_create_increasing)
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

Test(slab_group_create, group_create_deacreasing)
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

Test(slab_group_create, group_create_duplicate)
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
    cr_assert_not_null(parser);
    cr_assert_eq(parser->size_multiplicity, 1);
    parser = parser->next;
    cr_assert_not_null(parser);
    cr_assert_eq(parser->size_multiplicity, 2);
    parser = parser->next;
    cr_assert_not_null(parser);
    cr_assert_eq(parser->size_multiplicity, 3);
    cr_assert_eq(parser->next, NULL);

    // Decreasing order
    parser = parser->prev;
    cr_assert_not_null(parser);
    cr_assert_eq(parser->size_multiplicity, 2);
    parser = parser->prev;
    cr_assert_not_null(parser);
    cr_assert_eq(parser->size_multiplicity, 1);
    parser = parser->prev;
    cr_assert_not_null(parser);
    cr_assert_eq(parser->size_multiplicity, 0);
    cr_assert_eq(parser->prev, NULL);

    slab_group_destroy_all(group);
}

Test(slab_group_delete, delete_first)
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

Test(slab_group_delete, delete_middle)
{
    struct slab_group *group = slab_group_create(0, NULL);
    group = slab_group_create(1, group);
    group = slab_group_create(2, group);
    group = slab_group_create(3, group);

    // Delete group
    group = slab_group_delete(group->next->next);

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

Test(slab_group_delete, delete_last)
{
    struct slab_group *group = slab_group_create(0, NULL);
    group = slab_group_create(1, group);
    group = slab_group_create(2, group);
    group = slab_group_create(3, group);

    group = slab_group_delete(group->next->next->next);

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
