#include <criterion/criterion.h>

#include "cache.h"
#include "slab.h"

Test(cache_add_data, add_null)
{
    struct slab_cache cache = { 0 };
    cache_add_data(&cache, NULL, 0);
    cr_assert_eq(cache.nb_cached_slabs, 0);
}

Test(cache_add_data, add_null_cache)
{
    // Must not crash
    cache_add_data(NULL, NULL, 0);
}

Test(cache_add_data, add_data)
{
    struct slab_cache cache = { 0 };
    struct slab_meta slab_meta = { .nb_used_slabs = 24 };

    uint64_t free_bit_index = 32;

    // ------------------------------------------------------------------------

    cache_add_data(&cache, &slab_meta, free_bit_index);
    cr_assert_eq(cache.nb_cached_slabs, 1);

    cr_assert_eq(cache.cached_slabs[0].slab_meta, &slab_meta);
    cr_assert_eq(cache.cached_slabs[0].slab_meta->nb_used_slabs, 24);

    cr_assert_eq(cache.cached_slabs[0].free_bit_index, free_bit_index);
    cr_assert_eq(cache.cached_slabs[0].free_bit_index, 32);

    // ------------------------------------------------------------------------

    cache_add_data(&cache, &slab_meta, free_bit_index);
    cr_assert_eq(cache.nb_cached_slabs, 2);

    cr_assert_eq(cache.cached_slabs[1].slab_meta, &slab_meta);
    cr_assert_eq(cache.cached_slabs[1].slab_meta->nb_used_slabs, 24);

    cr_assert_eq(cache.cached_slabs[1].free_bit_index, free_bit_index);
    cr_assert_eq(cache.cached_slabs[1].free_bit_index, 32);

    // ------------------------------------------------------------------------

    cache_add_data(&cache, &slab_meta, free_bit_index);
    cr_assert_eq(cache.nb_cached_slabs, 3);

    cr_assert_eq(cache.cached_slabs[2].slab_meta, &slab_meta);
    cr_assert_eq(cache.cached_slabs[2].slab_meta->nb_used_slabs, 24);

    cr_assert_eq(cache.cached_slabs[2].free_bit_index, free_bit_index);
    cr_assert_eq(cache.cached_slabs[2].free_bit_index, 32);
}

Test(cache_add_data, add_full)
{
    struct slab_cache cache = { 0 };
    struct slab_meta slab_meta;
    uint64_t free_bit_index = 666;

    for (uint8_t i = 0; i < NB_CACHED_ENTRY + 42; i++)
    {
        cache_add_data(&cache, &slab_meta, free_bit_index);
    }

    cr_assert_eq(cache.nb_cached_slabs, NB_CACHED_ENTRY);
}

Test(cache_delete_by_index, delete_single)
{
    struct slab_cache cache = { 0 };
    struct slab_meta slab_meta = { .nb_used_slabs = 24 };

    cache_add_data(&cache, &slab_meta, 32);

    cache_delete_by_index(&cache, 0);

    cr_assert_eq(cache.nb_cached_slabs, 0);
}

Test(cache_delete_by_index, delete_last_second)
{
    struct slab_cache cache = { 0 };
    struct slab_meta slab_meta = { .nb_used_slabs = 24 };

    cache_add_data(&cache, &slab_meta, 0);
    cache_add_data(&cache, &slab_meta, 1);

    cache_delete_by_index(&cache, 1);

    cr_assert_eq(cache.nb_cached_slabs, 1);
    cr_assert_eq(cache.cached_slabs[0].free_bit_index, 0);
}

Test(cache_delete_by_index, delete_last_third)
{
    struct slab_cache cache = { 0 };
    struct slab_meta slab_meta = { .nb_used_slabs = 24 };

    cache_add_data(&cache, &slab_meta, 0);
    cache_add_data(&cache, &slab_meta, 1);
    cache_add_data(&cache, &slab_meta, 2);

    cache_delete_by_index(&cache, 2);

    cr_assert_eq(cache.nb_cached_slabs, 2);
    cr_assert_eq(cache.cached_slabs[0].free_bit_index, 0);
    cr_assert_eq(cache.cached_slabs[1].free_bit_index, 1);
}

Test(cache_delete_by_index, delete_first_3elt)
{
    struct slab_cache cache = { 0 };
    struct slab_meta slab_meta = { .nb_used_slabs = 24 };

    cache_add_data(&cache, &slab_meta, 0);
    cache_add_data(&cache, &slab_meta, 1);
    cache_add_data(&cache, &slab_meta, 2);

    cache_delete_by_index(&cache, 0);

    cr_assert_eq(cache.nb_cached_slabs, 2);
    cr_assert_eq(cache.cached_slabs[0].free_bit_index, 1);
    cr_assert_eq(cache.cached_slabs[1].free_bit_index, 2);
}

Test(cache_delete_by_index, delete_middle)
{
    struct slab_cache cache = { 0 };
    struct slab_meta slab_meta = { .nb_used_slabs = 24 };

    cache_add_data(&cache, &slab_meta, 1);
    cache_add_data(&cache, &slab_meta, 2);
    cache_add_data(&cache, &slab_meta, 3);

    cache_delete_by_index(&cache, 1);

    cr_assert_eq(cache.nb_cached_slabs, 2, "nb_cached_slabs: %d",
                 cache.nb_cached_slabs);
    cr_assert_eq(cache.cached_slabs[0].free_bit_index, 1);
    cr_assert_eq(cache.cached_slabs[1].free_bit_index, 3);
}
