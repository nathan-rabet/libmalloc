#include <criterion/criterion.h>

#include "cast.h"
#include "maths.h"
#include "slab.h"

Test(slab_data_from_meta_index, offset_i)
{
    struct slab_group *slab_group = slab_group_create(2, NULL);

    slab_group->slabs_meta =
        slab_meta_create(slab_group->slabs_meta, slab_group);
    for (size_t i = 0; i < MAX_META_SLAB_USED - 1; i++)
    {
        struct slab_data *addr =
            slab_data_from_meta_index(slab_group->slabs_meta, i);
        cr_assert_eq(addr,
                     (char *)slab_group->slabs_meta->slabs_data
                         + i * get_slab_size(slab_group->slabs_meta));
    }

    slab_group_destroy_all();
}

Test(slab_data_from_meta_index, offset_end)
{
    struct slab_group *slab_group = slab_group_create(2, NULL);

    slab_group->slabs_meta =
        slab_meta_create(slab_group->slabs_meta, slab_group);

    struct slab_data *addr = slab_data_from_meta_index(slab_group->slabs_meta,
                                                       MAX_META_SLAB_USED - 1);
    cr_assert_eq(addr,
                 (char *)slab_group->slabs_meta->slabs_data
                     + get_meta_size(slab_group->slabs_meta)
                     - get_slab_size(slab_group->slabs_meta));

    slab_group_destroy_all();
}

Test(slab_data_from_meta_index, offset_out_of_range, .disabled = true)
{
    struct slab_group *slab_group = slab_group_create(2, NULL);

    slab_group->slabs_meta =
        slab_meta_create(slab_group->slabs_meta, slab_group);

    struct slab_data *addr =
        slab_data_from_meta_index(slab_group->slabs_meta, MAX_META_SLAB_USED);
    cr_assert_null(addr);

    slab_group_destroy_all();
}

Test(slab_data_from_meta_index, null_param, .disabled = true)
{
    struct slab_data *addr = slab_data_from_meta_index(NULL, 0);
    cr_assert_null(addr);
}

#ifndef DEBUG
Test(slab_data_init, null)
{
    // Must not crash
    slab_data_init(NULL, 0);
}
#endif

Test(slab_data_init, basic)
{
    struct slab_group *slab_group = slab_group_create(2, NULL);

    slab_group->slabs_meta =
        slab_meta_create(slab_group->slabs_meta, slab_group);

    for (size_t i = 0; i < MAX_META_SLAB_USED; i++)
    {
        slab_data_init(slab_group->slabs_meta, i);

        struct slab_data *slab_data_i =
            slab_data_from_meta_index(slab_group->slabs_meta, i);

        cr_assert_eq(
            slab_data_i->canary_head,
            (CANARY_HEAD_FUNCTION((size_t)slab_data_i->my_meta_with_offset)));

        cr_assert_eq(
            slab_data_i->canary_tail,
            (CANARY_TAIL_FUNCTION((size_t)slab_data_i->my_meta_with_offset)));

        cr_assert_eq(slab_data_i->my_meta_with_offset,
                     slab_group->slabs_meta->slab_allocated + i);
    }

    slab_group_destroy_all();
}

Test(coin_coin, valid)
{
    struct slab_group *slab_group = slab_group_create(2, NULL);

    slab_group->slabs_meta =
        slab_meta_create(slab_group->slabs_meta, slab_group);

    for (size_t i = 0; i < MAX_META_SLAB_USED; i++)
    {
        slab_data_init(slab_group->slabs_meta, i);

        struct slab_data *slab_data_i =
            slab_data_from_meta_index(slab_group->slabs_meta, i);

        cr_assert_eq(coin_coin(slab_data_i), true);
    }

    slab_group_destroy_all();
}

Test(coin_coin, not_valid)
{
    struct slab_group *slab_group = slab_group_create(2, NULL);

    slab_group->slabs_meta =
        slab_meta_create(slab_group->slabs_meta, slab_group);

    slab_data_init(slab_group->slabs_meta, 0);

    struct slab_data *slab_data_i =
        slab_data_from_meta_index(slab_group->slabs_meta, 0);

    slab_data_i->canary_head = 0xDEADBEEF;
    cr_assert_eq(coin_coin(slab_data_i), false);

    slab_data_i->canary_tail = 0xDADFEED0;
    cr_assert_eq(coin_coin(slab_data_i), false);

    slab_group_destroy_all();
}

#ifndef DEBUG
Test(slab_data_free, null)
{
    // Must not crash
    slab_data_free(NULL);
}
#endif

Test(slab_data_bench, has_enough_space)
{
    for (size_t i = 0; i < 20; i++)
    {
        struct slab_group *slab_group = slab_group_create(i, NULL);

        for (size_t j = 0; j < slab_group->slabs_meta->max_handled_slabs; j++)
        {
            slab_meta_allocate(slab_group->slabs_meta, false);
        }

        size_t slab_size = power_2(i);
        for (size_t j = 0; j < slab_group->slabs_meta->max_handled_slabs; j++)
        {
            for (size_t k = 0; k < slab_size; k++)
            {
                cr_assert_eq(
                    slab_data_from_meta_index(slab_group->slabs_meta, j)
                        ->data[k],
                    0, "With a size of %zu, the data is not empty at index %zu",
                    slab_size, k);
            }
        }
    }

    slab_group_destroy_all();
}
