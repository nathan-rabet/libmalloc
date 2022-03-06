#include <criterion/criterion.h>
#include <math.h>

#include "maths.h"
#include "slab.h"

Test(slab_meta_size, slab_raw_size)
{
    struct slab_group *slab_group = slab_group_create(2, NULL);

    size_t slab_size = get_slab_raw_size(slab_group->slabs_meta);
    cr_assert_eq(slab_size, 4, "Slab RAW size is %d", slab_size);

    slab_group_destroy_all(slab_group);
}

Test(slab_meta_size, slab_size)
{
    struct slab_group *slab_group = slab_group_create(2, NULL);

    size_t slab_size = get_slab_size(slab_group->slabs_meta);
    cr_assert_eq(slab_size, SLAB_HEADER_DATA_SIZE + 4, "Slab TOTAL size is %d",
                 slab_size);

    slab_group_destroy_all(slab_group);
}

Test(slab_meta_size, meta_total_size)
{
    struct slab_group *slab_group = slab_group_create(2, NULL);

    size_t slab_size = get_meta_size(slab_group->slabs_meta);
    cr_assert_eq(slab_size, MAX_META_SLAB_USED * (SLAB_HEADER_DATA_SIZE + 4),
                 "Slabs sizes are %d", slab_size);

    slab_group_destroy_all(slab_group);
}

Test(slab_meta_create, create_basic)
{
    struct slab_group *slab_group = slab_group_create(2, NULL);

    slab_group->slabs_meta =
        slab_meta_create(slab_group->slabs_meta, slab_group);

    // First meta
    cr_assert_not_null(slab_group->slabs_meta);
    cr_assert_null(slab_group->slabs_meta->prev);
    cr_assert_not_null(slab_group->slabs_meta->next);

    // Second meta
    cr_assert_null(slab_group->slabs_meta->next->next);
    cr_assert_not_null(slab_group->slabs_meta->next->prev);

    cr_assert_eq(slab_group->slabs_meta->common_group, slab_group);
    cr_assert_not_null(slab_group->slabs_meta->slabs_data);
    cr_assert_eq(slab_group->slabs_meta->slab_used_len, MAX_META_SLAB_USED,
                 "Slab used len is %d", slab_group->slabs_meta->slab_used_len);

    slab_group_destroy_all(slab_group);
}

Test(slab_meta_create, create_no_group)
{
    struct slab_group *slab_group = slab_group_create(2, NULL);

    cr_assert_null(slab_meta_create(slab_group->slabs_meta, NULL));

    slab_group_destroy_all(slab_group);
}

Test(slab_meta_create, create_null_arguments)
{
    cr_assert_null(slab_meta_create(NULL, NULL));
}

Test(slab_meta_create, logarithmic_no_decrease)
{
    struct slab_group *slab_group = slab_group_create(
        (floor(log2(LOGARITHMIC_DECREASE_BYTES_THRESHOLD / MAX_META_SLAB_USED))
         - 1),
        NULL);

    cr_assert_eq(slab_group->slabs_meta->slab_used_len, MAX_META_SLAB_USED,
                 "Slab used len is %ld", slab_group->slabs_meta->slab_used_len);

    slab_group_destroy_all(slab_group);
}

// ! FIXME: This test is not working
Test(slab_meta_create, logarithmic_decreases, .disabled = true)
{
    for (size_t i = 0; i < MAX_META_SLAB_USED; i++)
    {
        struct slab_group *slab_group =
            slab_group_create(floor(log2(LOGARITHMIC_DECREASE_BYTES_THRESHOLD
                                         / MAX_META_SLAB_USED))
                                  + i,
                              NULL);

        cr_assert_eq(slab_group->slabs_meta->slab_used_len,
                     MAX(1, MAX_META_SLAB_USED - 1 - i), "Slab used len is %ld",
                     slab_group->slabs_meta->slab_used_len);
        slab_group_destroy_all(slab_group);
    }
}

Test(slab_meta_delete, delete_nothing)
{
    cr_assert_null(slab_meta_delete(NULL));
}

Test(slab_meta_delete, delete_single)
{
    struct slab_group *slab_group = slab_group_create(2, NULL);

    slab_group->slabs_meta = slab_meta_delete(slab_group->slabs_meta);

    cr_assert_null(slab_group->slabs_meta);

    slab_group_destroy_all(slab_group);
}

Test(slab_meta_delete, delete_first)
{
    struct slab_group *slab_group = slab_group_create(2, NULL);

    slab_group->slabs_meta =
        slab_meta_create(slab_group->slabs_meta, slab_group);

    slab_group->slabs_meta = slab_meta_delete(slab_group->slabs_meta);

    cr_assert_not_null(slab_group->slabs_meta);
    cr_assert_null(slab_group->slabs_meta->prev);
    cr_assert_null(slab_group->slabs_meta->next);

    slab_group_destroy_all(slab_group);
}

Test(slab_meta_delete, delete_second)
{
    struct slab_group *slab_group = slab_group_create(2, NULL);

    slab_group->slabs_meta =
        slab_meta_create(slab_group->slabs_meta, slab_group);

    struct slab_meta *meta = slab_group->slabs_meta;

    slab_meta_delete(slab_group->slabs_meta->next);

    cr_assert_eq(slab_group->slabs_meta, meta);
    cr_assert_null(slab_group->slabs_meta->prev);
    cr_assert_null(slab_group->slabs_meta->next);

    slab_group_destroy_all(slab_group);
}

Test(slab_meta_delete, delete_middle)
{
    struct slab_group *slab_group = slab_group_create(2, NULL);

    struct slab_meta *slab_meta_3 = slab_group->slabs_meta;
    slab_group->slabs_meta =
        slab_meta_create(slab_group->slabs_meta, slab_group);
    struct slab_meta *slab_meta_2 = slab_group->slabs_meta;
    slab_group->slabs_meta =
        slab_meta_create(slab_group->slabs_meta, slab_group);
    struct slab_meta *slab_meta_1 = slab_group->slabs_meta;

    // slab_meta_2 is the middle one
    slab_group->slabs_meta = slab_meta_delete(slab_meta_2);

    cr_assert_eq(slab_group->slabs_meta, slab_meta_1);
    cr_assert_eq(slab_group->slabs_meta->next, slab_meta_3);
    cr_assert_null(slab_group->slabs_meta->prev);
    cr_assert_null(slab_group->slabs_meta->next->next);
    slab_group_destroy_all(slab_group);
}

Test(slab_meta_free, free_null)
{
    cr_assert_eq(slab_meta_free(NULL, 0), false);
}

Test(slab_meta_free, free_wrong_index)
{
    struct slab_group *slab_group = slab_group_create(2, NULL);

    cr_assert_eq(slab_meta_free(slab_group->slabs_meta, MAX_META_SLAB_USED + 1),
                 false);

    cr_assert_eq(slab_group->slabs_meta->slab_used_len, MAX_META_SLAB_USED,
                 "Slab used len is %ld", slab_group->slabs_meta->slab_used_len);

    slab_group_destroy_all(slab_group);
}

Test(slab_meta_free, free_wrong_index_2)
{
    struct slab_group *slab_group = slab_group_create(
        floor(log2(LOGARITHMIC_DECREASE_BYTES_THRESHOLD / MAX_META_SLAB_USED)),
        NULL);

    slab_data_init(slab_group->slabs_meta,
                   slab_group->slabs_meta->slab_used_len - 1);

    cr_assert_eq(slab_meta_free(slab_group->slabs_meta, MAX_META_SLAB_USED - 1),
                 false);

    cr_assert_eq(slab_group->slabs_meta->slab_used_len, MAX_META_SLAB_USED - 1,
                 "Slab used len is %ld", slab_group->slabs_meta->slab_used_len);

    slab_group_destroy_all(slab_group);
}

Test(slab_meta_free, free_already_free)
{
    struct slab_group *slab_group = slab_group_create(2, NULL);

    cr_assert_eq(slab_meta_free(slab_group->slabs_meta, 0), false);

    cr_assert_eq(slab_group->slabs_meta->slab_used_len, MAX_META_SLAB_USED,
                 "Slab used len is %ld", slab_group->slabs_meta->slab_used_len);

    slab_group_destroy_all(slab_group);
}

Test(slab_meta_free, free_no_munmap)
{
    struct slab_group *slab_group = slab_group_create(2, NULL);

    slab_meta_allocate(slab_group->slabs_meta, false);
    slab_meta_allocate(slab_group->slabs_meta, false);

    cr_assert_eq(slab_meta_free(slab_group->slabs_meta, 0), true);
    cr_assert_not_null(slab_group->slabs_meta);

    slab_group_destroy_all(slab_group);
}

Test(slab_meta_free, free_munmap)
{
    struct slab_group *slab_group = slab_group_create(2, NULL);

    for (size_t i = 0; i < MAX_META_SLAB_USED + 1; i++)
        slab_meta_allocate(slab_group->slabs_meta, false);

    cr_assert_eq(slab_meta_free(slab_group->slabs_meta, 0), true);
    cr_assert_null(slab_group->slabs_meta->next);

    slab_group_destroy_all(slab_group);
}

Test(slab_meta_retreive_index, retreive_slab_meta_index_null)
{
    cr_assert_eq(slab_meta_retreive_index(NULL), 0);
}

Test(slab_meta_retreive_index, indexes)
{
    struct slab_group *slab_group = slab_group_create(2, NULL);

    for (size_t i = 0; i < MAX_META_SLAB_USED; i++)
        cr_assert_eq(slab_meta_retreive_index(
                         slab_group->slabs_meta->slab_allocated + i),
                     i);

    slab_group_destroy_all(slab_group);
}
