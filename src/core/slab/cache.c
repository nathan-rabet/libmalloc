#include "cache.h"

void cache_add_data(struct slab_cache *cache, struct slab_meta *slab_meta,
                    uint64_t free_bit_index, bool is_dirty)
{
    if (cache && cache->nb_cached_slabs < NB_CACHED_ENTRY && slab_meta)
    {
        cache->cached_slabs[cache->nb_cached_slabs].slab_meta = slab_meta;
        cache->cached_slabs[cache->nb_cached_slabs].free_bit_index =
            free_bit_index;
        cache->cached_slabs[cache->nb_cached_slabs].is_dirty = is_dirty;
        cache->nb_cached_slabs++;
    }
}

void cache_delete_by_index(struct slab_cache *cache, uint8_t index)
{
    if (cache->nb_cached_slabs > 0 && index < cache->nb_cached_slabs)
    {
        switch (index)
        {
        case 0:
            cache->cached_slabs[0] = cache->cached_slabs[1];
            cache->cached_slabs[1] = cache->cached_slabs[2];
            break;

        case 1:
            cache->cached_slabs[1] = cache->cached_slabs[2];
            break;

        case 2:
        default:
            break;
        }

        cache->nb_cached_slabs--;
    }
}

int8_t cache_find_by_virginity(struct slab_cache *cache, bool is_dirty)
{
    for (int8_t i; cache && i < cache->nb_cached_slabs; i++)
        if (cache->cached_slabs[i].is_dirty == is_dirty)
            return i;

    return -1;
}

int8_t cache_find_must_be_virgin(struct slab_cache *cache, bool must_be_virgin)
{
    for (int8_t i; cache && i < cache->nb_cached_slabs; i++)
        if (!must_be_virgin
            || cache->cached_slabs[i].is_dirty == !must_be_virgin)
            return i;

    return -1;
}

void delete_all_occ_meta(struct slab_cache *cache, struct slab_meta *slab_meta)
{
    for (int8_t i = 0; cache && i < cache->nb_cached_slabs; i++)
        if (cache->cached_slabs[i].slab_meta == slab_meta)
        {
            cache_delete_by_index(cache, i);
            i = -1;
        }
}
