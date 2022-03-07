#include <criterion/criterion.h>

#include "cache.h"
#include "slab.h"

Test(cache_add_data, add_null)
{
    struct slab_cache cache = { 0 };
    cache_add_data(&cache, NULL, 0, false);
    cr_assert_eq(cache.nb_cached_slabs, 0);
}

Test(cache_add_data, add_null_cache)
{
    // Must not crash
    cache_add_data(NULL, NULL, 0, false);
}

Test(cache_add_data, add_data)
{
    struct slab_cache cache = { 0 };

    struct slab_meta slab_meta = { .nb_allocated_slabs = 24 };
    uint64_t free_bit_index = 32;
    bool is_dirty = true;

    // ------------------------------------------------------------------------

    cache_add_data(&cache, &slab_meta, free_bit_index, is_dirty);
    cr_assert_eq(cache.nb_cached_slabs, 1);

    cr_assert_eq(cache.cached_slabs[0].slab_meta, &slab_meta);
    cr_assert_eq(cache.cached_slabs[0].slab_meta->nb_allocated_slabs, 24);

    cr_assert_eq(cache.cached_slabs[0].free_bit_index, free_bit_index);
    cr_assert_eq(cache.cached_slabs[0].free_bit_index, 32);
    cr_assert_eq(cache.cached_slabs[0].is_dirty, is_dirty);

    // ------------------------------------------------------------------------

    cache_add_data(&cache, &slab_meta, free_bit_index, is_dirty);
    cr_assert_eq(cache.nb_cached_slabs, 2);

    cr_assert_eq(cache.cached_slabs[1].slab_meta, &slab_meta);
    cr_assert_eq(cache.cached_slabs[1].slab_meta->nb_allocated_slabs, 24);

    cr_assert_eq(cache.cached_slabs[1].free_bit_index, free_bit_index);
    cr_assert_eq(cache.cached_slabs[1].free_bit_index, 32);
    cr_assert_eq(cache.cached_slabs[1].is_dirty, is_dirty);

    // ------------------------------------------------------------------------

    cache_add_data(&cache, &slab_meta, free_bit_index, is_dirty);
    cr_assert_eq(cache.nb_cached_slabs, 3);

    cr_assert_eq(cache.cached_slabs[2].slab_meta, &slab_meta);
    cr_assert_eq(cache.cached_slabs[2].slab_meta->nb_allocated_slabs, 24);

    cr_assert_eq(cache.cached_slabs[2].free_bit_index, free_bit_index);
    cr_assert_eq(cache.cached_slabs[2].free_bit_index, 32);
    cr_assert_eq(cache.cached_slabs[2].is_dirty, is_dirty);
}

Test(cache_add_data, add_full)
{
    struct slab_cache cache = { 0 };
    struct slab_meta slab_meta;
    uint64_t free_bit_index = 666;

    for (uint8_t i = 0; i < NB_CACHED_ENTRY + 42; i++)
    {
        cache_add_data(&cache, &slab_meta, free_bit_index, false);
    }

    cr_assert_eq(cache.nb_cached_slabs, NB_CACHED_ENTRY);
}

Test(cache_delete_by_index, delete_single)
{
    struct slab_cache cache = { 0 };
    struct slab_meta slab_meta = { .max_handled_slabs = 24 };

    cache_add_data(&cache, &slab_meta, 32, false);

    cache_delete_by_index(&cache, 0);

    cr_assert_eq(cache.nb_cached_slabs, 0);
}

Test(cache_delete_by_index, delete_last_second)
{
    struct slab_cache cache = { 0 };
    struct slab_meta slab_meta = { .max_handled_slabs = 24 };
    bool is_dirty = true;

    cache_add_data(&cache, &slab_meta, 0, is_dirty);
    cache_add_data(&cache, &slab_meta, 1, is_dirty);

    cache_delete_by_index(&cache, 1);

    cr_assert_eq(cache.nb_cached_slabs, 1);
    cr_assert_eq(cache.cached_slabs[0].free_bit_index, 0);
    cr_assert_eq(cache.cached_slabs[0].is_dirty, is_dirty);
}

Test(cache_delete_by_index, delete_last_third)
{
    struct slab_cache cache = { 0 };
    struct slab_meta slab_meta = { .max_handled_slabs = 24 };
    bool is_dirty = true;

    cache_add_data(&cache, &slab_meta, 0, is_dirty);
    cache_add_data(&cache, &slab_meta, 1, is_dirty);
    cache_add_data(&cache, &slab_meta, 2, is_dirty);

    cache_delete_by_index(&cache, 2);

    cr_assert_eq(cache.nb_cached_slabs, 2);
    cr_assert_eq(cache.cached_slabs[0].free_bit_index, 0);
    cr_assert_eq(cache.cached_slabs[1].free_bit_index, 1);
}

Test(cache_delete_by_index, delete_first_3elt)
{
    struct slab_cache cache = { 0 };
    struct slab_meta slab_meta = { .max_handled_slabs = 24 };
    bool is_dirty = true;

    cache_add_data(&cache, &slab_meta, 0, is_dirty);
    cache_add_data(&cache, &slab_meta, 1, is_dirty);
    cache_add_data(&cache, &slab_meta, 2, is_dirty);

    cache_delete_by_index(&cache, 0);

    cr_assert_eq(cache.nb_cached_slabs, 2);
    cr_assert_eq(cache.cached_slabs[0].free_bit_index, 1);
    cr_assert_eq(cache.cached_slabs[1].free_bit_index, 2);
}

Test(cache_delete_by_index, delete_middle)
{
    struct slab_cache cache = { 0 };
    struct slab_meta slab_meta = { .max_handled_slabs = 24 };
    bool is_dirty = true;

    cache_add_data(&cache, &slab_meta, 1, is_dirty);
    cache_add_data(&cache, &slab_meta, 2, is_dirty);
    cache_add_data(&cache, &slab_meta, 3, is_dirty);

    cache_delete_by_index(&cache, 1);

    cr_assert_eq(cache.nb_cached_slabs, 2, "nb_cached_slabs: %d",
                 cache.nb_cached_slabs);
    cr_assert_eq(cache.cached_slabs[0].free_bit_index, 1);
    cr_assert_eq(cache.cached_slabs[1].free_bit_index, 3);
}

Test(cache_find_by_virginity, null_params)
{
    int8_t index = cache_find_by_virginity(NULL, NULL);
    cr_assert_eq(index, -1);
}

Test(cache_find_by_virginity, size_0)
{
    struct slab_cache cache = { 0 };
    bool is_dirty = true;

    int8_t index = cache_find_by_virginity(&cache, is_dirty);
    cr_assert_eq(index, -1);
}

Test(cache_find_by_virginity, find_first)
{
    struct slab_cache cache = { 0 };
    struct slab_meta slab_meta = { .max_handled_slabs = 24 };
    bool is_dirty = true;

    cache_add_data(&cache, &slab_meta, 0, is_dirty);

    int8_t index = cache_find_by_virginity(&cache, is_dirty);
    cr_assert_eq(index, 0);
}

Test(cache_find_by_virginity, find_second)
{
    struct slab_cache cache = { 0 };
    struct slab_meta slab_meta = { .max_handled_slabs = 24 };
    bool is_dirty = true;

    cache_add_data(&cache, &slab_meta, 0, !is_dirty);
    cache_add_data(&cache, &slab_meta, 1, is_dirty);

    int8_t index = cache_find_by_virginity(&cache, is_dirty);
    cr_assert_eq(index, 1, "index: %d", index);
}

Test(cache_find_by_virginity, find_last)
{
    struct slab_cache cache = { 0 };
    struct slab_meta slab_meta = { .max_handled_slabs = 24 };
    bool is_dirty = true;

    cache_add_data(&cache, &slab_meta, 0, !is_dirty);
    cache_add_data(&cache, &slab_meta, 1, !is_dirty);
    cache_add_data(&cache, &slab_meta, 2, is_dirty);

    int8_t index = cache_find_by_virginity(&cache, is_dirty);
    cr_assert_eq(index, 2, "index: %d", index);
}

Test(cache_find_must_be_virgin, null_params)
{
    int8_t index = cache_find_must_be_virgin(NULL, false);
    cr_assert_eq(index, -1);
}

Test(cache_find_must_be_virgin, size_0)
{
    struct slab_cache cache = { 0 };

    int8_t index = cache_find_must_be_virgin(&cache, true);
    cr_assert_eq(index, -1);
}

Test(cache_find_must_be_virgin, gargage_is_enough)
{
    struct slab_cache cache = { 0 };
    struct slab_meta slab_meta = { .max_handled_slabs = 24 };
    bool is_dirty = true;

    cache_add_data(&cache, &slab_meta, 0, is_dirty);

    int8_t index = cache_find_must_be_virgin(&cache, false);
    cr_assert_eq(index, 0);
}

Test(cache_find_must_be_virgin, gargage_is_enough_got_virgin)
{
    struct slab_cache cache = { 0 };
    struct slab_meta slab_meta = { .max_handled_slabs = 24 };
    bool is_dirty = false;

    cache_add_data(&cache, &slab_meta, 0, is_dirty);

    int8_t index = cache_find_must_be_virgin(&cache, false);
    cr_assert_eq(index, 0);
}

Test(cache_find_must_be_virgin, chad_want_virgin_and_always_find_one)
{
    struct slab_cache cache = { 0 };
    struct slab_meta slab_meta = { .max_handled_slabs = 24 };
    bool is_dirty = false;

    cache_add_data(&cache, &slab_meta, 0, is_dirty);

    int8_t index = cache_find_must_be_virgin(&cache, true);
    cr_assert_eq(index, 0);
}

Test(cache_find_must_be_virgin, want_virgin_but_u_are_simp)
{
    struct slab_cache cache = { 0 };
    struct slab_meta slab_meta = { .max_handled_slabs = 24 };
    bool is_dirty = true;

    cache_add_data(&cache, &slab_meta, 0, is_dirty);

    int8_t index = cache_find_must_be_virgin(&cache, true);
    cr_assert_eq(index, -1);
}

Test(cache_find_by_slab_meta, null_params)
{
    int8_t index = cache_find_by_slab_meta(NULL, NULL);
    cr_assert_eq(index, -1);
}

Test(cache_find_by_slab_meta, find_by_null)
{
    struct slab_cache cache = { 0 };
    struct slab_meta slab_meta;

    cache_add_data(&cache, &slab_meta, 32, false);

    int8_t index = cache_find_by_slab_meta(&cache, NULL);
    cr_assert_eq(index, -1);
}

Test(cache_find_by_slab_meta, find_first)
{
    struct slab_cache cache = { 0 };
    struct slab_meta slab_meta;

    cache_add_data(&cache, &slab_meta, 32, false);

    int8_t index = cache_find_by_slab_meta(&cache, &slab_meta);
    cr_assert_eq(index, 0);
}

Test(cache_find_by_slab_meta, find_second)
{
    struct slab_cache cache = { 0 };
    struct slab_meta slab_meta;

    cache_add_data(&cache, &slab_meta, 32, false);
    cache.cached_slabs[0].slab_meta = NULL;
    cache.cached_slabs[0].free_bit_index = 0;
    cache_add_data(&cache, &slab_meta, 32, false);

    int8_t index = cache_find_by_slab_meta(&cache, &slab_meta);
    cr_assert_eq(index, 1, "index: %d", index);
}

Test(cache_find_by_slab_meta, find_last)
{
    struct slab_cache cache = { 0 };
    struct slab_meta slab_meta;

    cache_add_data(&cache, &slab_meta, 32, false);
    cache_add_data(&cache, &slab_meta, 32, false);
    cache.cached_slabs[0].slab_meta = NULL;
    cache.cached_slabs[1].slab_meta = NULL;

    cache_add_data(&cache, &slab_meta, 32, false);

    int8_t index = cache_find_by_slab_meta(&cache, &slab_meta);
    cr_assert_eq(index, 2, "index: %d", index);
}
