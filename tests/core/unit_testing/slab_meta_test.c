#include <criterion/criterion.h>
#include <math.h>

#include "slab.h"

Test(slab_meta, meta_size)
{
    struct slab_group *slab_group = slab_group_create(2, NULL);

    size_t slab_size = get_meta_size(slab_group->slabs_meta);
    cr_assert_eq(slab_size, MAX_META_SLAB_USED * (SLAB_HEADER_DATA_SIZE + 4),
                 "Slab size is %d", slab_size);

    slab_group_destroy_all(slab_group);
}

Test(slab_meta, create_basic)
{
    struct slab_group *slab_group = slab_group_create(2, NULL);

    slab_group->slabs_meta =
        slab_meta_create(slab_group->slabs_meta, slab_group);

    cr_assert_not_null(slab_group->slabs_meta);
    cr_assert_eq(slab_group->slabs_meta->common_group, slab_group);
    cr_assert_not_null(slab_group->slabs_meta->slabs_data);
    cr_assert_eq(slab_group->slabs_meta->slab_used_len, MAX_META_SLAB_USED,
                 "Slab used len is %d", slab_group->slabs_meta->slab_used_len);

    slab_group_destroy_all(slab_group);
}

Test(slab_meta, create_no_group)
{
    struct slab_group *slab_group = slab_group_create(2, NULL);

    cr_assert_null(slab_meta_create(slab_group->slabs_meta, NULL));
}

Test(slab_meta, create_null_arguments)
{
    cr_assert_null(slab_meta_create(NULL, NULL));
}

Test(slab_meta, logarithmic_decrease_0)
{
    struct slab_group *slab_group = slab_group_create(
        floor(log2(LOGARITHMIC_DECREASE_BYTES_THRESHOLD / MAX_META_SLAB_USED
                   - 1)),
        NULL);

    cr_assert_eq(slab_group->slabs_meta->slab_used_len, MAX_META_SLAB_USED,
                 "Slab used len is %ld", slab_group->slabs_meta->slab_used_len);

    slab_group_destroy_all(slab_group);
}

Test(slab_meta, logarithmic_decrease_1)
{
    struct slab_group *slab_group = slab_group_create(
        floor(log2(LOGARITHMIC_DECREASE_BYTES_THRESHOLD)), NULL);

    cr_assert_lt(slab_group->slabs_meta->slab_used_len, MAX_META_SLAB_USED,
                 "Slab used len is %ld", slab_group->slabs_meta->slab_used_len);

    slab_group_destroy_all(slab_group);
}

Test(slab_meta, logarithmic_decrease_2)
{
    struct slab_group *slab_group = slab_group_create(
        floor(log2(16 * LOGARITHMIC_DECREASE_BYTES_THRESHOLD)), NULL);

    cr_assert_eq(slab_group->slabs_meta->slab_used_len, 1,
                 "Slab used len is %ld", slab_group->slabs_meta->slab_used_len);

    slab_group_destroy_all(slab_group);
}
