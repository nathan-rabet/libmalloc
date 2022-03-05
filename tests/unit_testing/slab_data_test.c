#include <criterion/criterion.h>

#include "slab.h"

Test(slab_data_addr_from_meta, offset0)
{
    struct slab_group *slab_group = slab_group_create(2, NULL);

    slab_group->slabs_meta =
        slab_meta_create(slab_group->slabs_meta, slab_group);

    struct slab_data *addr =
        slab_data_addr_from_meta(slab_group->slabs_meta, 0);
    cr_assert_eq(addr,
                 (char *)slab_group->slabs_meta->slabs_data
                     + 0 * get_slab_size(slab_group->slabs_meta));
    slab_group_destroy_all(slab_group);
}

Test(slab_data_addr_from_meta, offset1)
{
    struct slab_group *slab_group = slab_group_create(2, NULL);

    slab_group->slabs_meta =
        slab_meta_create(slab_group->slabs_meta, slab_group);

    struct slab_data *addr =
        slab_data_addr_from_meta(slab_group->slabs_meta, 1);
    cr_assert_eq(addr,
                 (char *)slab_group->slabs_meta->slabs_data
                     + get_slab_size(slab_group->slabs_meta),
                 "offset %zun must be 0",
                 slab_group->slabs_meta->slabs_data
                     + get_slab_size(slab_group->slabs_meta) - addr);

    slab_group_destroy_all(slab_group);
}

Test(slab_data_addr_from_meta, offset_end)
{
    struct slab_group *slab_group = slab_group_create(2, NULL);

    slab_group->slabs_meta =
        slab_meta_create(slab_group->slabs_meta, slab_group);

    struct slab_data *addr = slab_data_addr_from_meta(slab_group->slabs_meta,
                                                      MAX_META_SLAB_USED - 1);
    cr_assert_eq(addr,
                 (char *)slab_group->slabs_meta->slabs_data
                     + get_meta_size(slab_group->slabs_meta)
                     - get_slab_size(slab_group->slabs_meta));

    slab_group_destroy_all(slab_group);
}

Test(slab_data_addr_from_meta, offset_out_of_range)
{
    struct slab_group *slab_group = slab_group_create(2, NULL);

    slab_group->slabs_meta =
        slab_meta_create(slab_group->slabs_meta, slab_group);

    struct slab_data *addr =
        slab_data_addr_from_meta(slab_group->slabs_meta, MAX_META_SLAB_USED);
    cr_assert_null(addr);

    slab_group_destroy_all(slab_group);
}

Test(slab_data_addr_from_meta, null_param)
{
    struct slab_data *addr = slab_data_addr_from_meta(NULL, 0);
    cr_assert_null(addr);
}
