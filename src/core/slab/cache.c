#include "cache.h"

void cache_add_data(struct slab_cache *cache, struct slab_group *slab_group,
                    struct slab_meta *slab_meta, uint64_t free_bit_index)
{
    if (cache && cache->nb_cached_slabs < NB_CACHED_ENTRY && slab_group
        && slab_meta)
    {
        cache->cached_slabs[cache->nb_cached_slabs].slab_group = slab_group;
        cache->cached_slabs[cache->nb_cached_slabs].slab_meta = slab_meta;
        cache->cached_slabs[cache->nb_cached_slabs].free_bit_index =
            free_bit_index;
        cache->nb_cached_slabs++;
    }
}

int8_t cache_find_by_slab_group(struct slab_cache *cache,
                                struct slab_group *slab_group)
{
    for (int8_t i; cache && i < cache->nb_cached_slabs; i++)
        if (cache->cached_slabs[i].slab_group == slab_group)
            return i;

    return -1;
}

int8_t cache_find_by_slab_meta(struct slab_cache *cache,
                               struct slab_meta *slab_meta)
{
    for (int8_t i; cache && i < cache->nb_cached_slabs; i++)
        if (cache->cached_slabs[i].slab_meta == slab_meta)
            return i;

    return -1;
}

int8_t cache_find_by_free_bit_index(struct slab_cache *cache,
                                    size_t free_bit_index)
{
    for (int8_t i; cache && i < cache->nb_cached_slabs; i++)
        if (cache->cached_slabs[i].free_bit_index == free_bit_index)
            return i;

    return -1;
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
