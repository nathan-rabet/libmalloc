#include <criterion/criterion.h>

#include "maths.h"
#include "slab.h"

Test(slab_group_power, power_of_2)
{
    cr_assert_eq(power_2(0), 1);
    cr_assert_eq(power_2(1), 2);
    cr_assert_eq(power_2(2), 4);
    cr_assert_eq(power_2(3), 8);
    cr_assert_eq(power_2(4), 16);
    cr_assert_eq(power_2(5), 32);
}

Test(slab_group_create, group_create_check_meta)
{
    struct slab_group *group = slab_group_create(0, NULL);

    cr_assert_not_null(group);
    cr_assert_eq(group->size_multiplicity, 0);
    cr_assert_not_null(group->slabs_meta);
    cr_assert_eq(power_2(group->slabs_meta->common_group->size_multiplicity),
                 1);

    slab_group_destroy_all(group);
}

Test(slab_group_create, group_create_check_zero)
{
    struct slab_group *group = slab_group_create(1, NULL);

    cr_assert_not_null(group);
    cr_assert_eq(group->size_multiplicity, 1);
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

Test(slab_group_find_enough_space, find_enough_space_basic)
{
    struct slab_group *group = slab_group_create(0, NULL);
    group = slab_group_create(1, group);
    group = slab_group_create(2, group);
    group = slab_group_create(3, group);

    struct slab_group *found = slab_group_find_enough_space(group, 4);

    cr_assert_eq(found->size_multiplicity, 2);

    slab_group_destroy_all(group);
}

Test(slab_group_find_enough_space, first)
{
    struct slab_group *group = slab_group_create(0, NULL);
    group = slab_group_create(1, group);
    group = slab_group_create(2, group);
    group = slab_group_create(3, group);

    struct slab_group *found = slab_group_find_enough_space(group, 1);

    cr_assert_eq(found->size_multiplicity, 0);

    slab_group_destroy_all(group);
}

Test(slab_group_find_enough_space, last)
{
    struct slab_group *group = slab_group_create(0, NULL);
    group = slab_group_create(1, group);
    group = slab_group_create(2, group);
    group = slab_group_create(3, group);

    struct slab_group *found = slab_group_find_enough_space(group, 8);

    cr_assert_eq(found->size_multiplicity, 3);

    slab_group_destroy_all(group);
}

Test(slab_group_find_enough_space, not_perfect_log)
{
    struct slab_group *group = slab_group_create(0, NULL);
    group = slab_group_create(1, group);
    group = slab_group_create(2, group);
    group = slab_group_create(3, group);

    struct slab_group *found = slab_group_find_enough_space(group, 5);

    cr_assert_eq(found->size_multiplicity, 3);

    slab_group_destroy_all(group);
}

Test(slab_group_find_enough_space, not_found)
{
    struct slab_group *group = slab_group_create(0, NULL);
    group = slab_group_create(1, group);
    group = slab_group_create(2, group);
    group = slab_group_create(3, group);

    struct slab_group *found = slab_group_find_enough_space(group, 10);

    cr_assert_null(found);

    slab_group_destroy_all(group);
}

Test(slab_group_find_enough_space, not_found_2)
{
    struct slab_group *group = slab_group_create(0, NULL);
    group = slab_group_create(10, group);

    struct slab_group *found = slab_group_find_enough_space(group, 42);

    cr_assert_null(found);

    slab_group_destroy_all(group);
}

Test(slab_group_allocate, allocate_basic)
{
    struct slab_group *group = slab_group_create(0, NULL);
    group = slab_group_create(1, group);
    group = slab_group_create(2, group);
    group = slab_group_create(3, group);

    struct slab_group *found = slab_group_find_enough_space(group, 4);

    bool *ptr = slab_group_allocate(found, false);

    cr_assert_not_null(ptr);

    struct slab_meta *meta = page_begin(ptr);
    size_t index = slab_meta_retreive_index(ptr);

    cr_assert_eq(meta->slab_allocated[index], true);
    cr_assert_eq(meta->slab_dirty[index], true);

    slab_group_destroy_all(group);
}

Test(slab_group_allocate, allocate_new_meta)
{
    struct slab_group *group = slab_group_create(0, NULL);
    group = slab_group_create(1, group);
    group = slab_group_create(2, group);
    group = slab_group_create(3, group);

    struct slab_group *found = slab_group_find_enough_space(group, 4);
    struct slab_meta *meta_before = found->slabs_meta;

    // Bypass cache
    for (size_t i = 0; i < NB_CACHED_ENTRY; i++)
        slab_group_allocate(found, true);

    cr_assert_not_null(found);
    cr_assert_not_null(found->slabs_meta);
    cr_assert_not_null(found->slabs_meta->slab_allocated);

    for (size_t i = 0; i < MAX_META_SLAB_USED; i++)
        found->slabs_meta->slab_allocated[i] = true;

    bool *ptr = slab_group_allocate(found, false);

    cr_assert_not_null(ptr);

    struct slab_meta *meta_after = page_begin(ptr);
    size_t index = slab_meta_retreive_index(ptr);

    cr_assert_neq(meta_after, meta_before);
    cr_assert_eq(meta_after->slab_allocated[index], true);
    cr_assert_eq(meta_after->slab_dirty[index], true);

    slab_group_destroy_all(group);
}

Test(slab_group_allocate, virginity)
{
    struct slab_group *group = slab_group_create(0, NULL);
    group = slab_group_create(1, group);
    group = slab_group_create(2, group);
    group = slab_group_create(3, group);

    struct slab_group *found = slab_group_find_enough_space(group, 4);

    // Bypass cache
    for (size_t i = 0; i < NB_CACHED_ENTRY; i++)
        slab_group_allocate(found, true);

    for (size_t i = 0; i < MAX_META_SLAB_USED - 1; i++)
        found->slabs_meta->slab_dirty[i] = true;

    bool *ptr = slab_group_allocate(found, true);
    struct slab_meta *meta = page_begin(ptr);
    size_t index = slab_meta_retreive_index(ptr);

    cr_assert_eq(index, MAX_META_SLAB_USED - 1);
    cr_assert_eq(meta->slab_allocated[index], true);
    cr_assert_eq(meta->slab_dirty[index], true);

    slab_group_destroy_all(group);
}

Test(slab_group_allocate, not_used_and_virgin)
{
    struct slab_group *group = slab_group_create(0, NULL);
    group = slab_group_create(1, group);
    group = slab_group_create(2, group);
    group = slab_group_create(3, group);

    struct slab_group *found = slab_group_find_enough_space(group, 4);

    // Bypass cache
    for (size_t i = 0; i < NB_CACHED_ENTRY; i++)
        slab_group_allocate(found, true);

    for (size_t i = 0; i < MAX_META_SLAB_USED / 2; i++)
        found->slabs_meta->slab_allocated[i] = true;

    for (size_t i = MAX_META_SLAB_USED / 2; i < MAX_META_SLAB_USED - 1; i++)
        found->slabs_meta->slab_dirty[i] = true;

    bool *ptr = slab_group_allocate(found, true);
    struct slab_meta *meta = page_begin(ptr);
    size_t index = slab_meta_retreive_index(ptr);

    cr_assert_eq(index, MAX_META_SLAB_USED - 1, "index = %zu", index);
    cr_assert_eq(meta->slab_allocated[index], true);
    cr_assert_eq(meta->slab_dirty[index], true);

    slab_group_destroy_all(group);
}
