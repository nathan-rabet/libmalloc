#ifndef CACHE_H
#define CACHE_H

#include <stdint.h>

struct slab_cache
{
    uint8_t nb_cached_slabs : 2; // Number of cached slabs (0-3)
    struct slab_cache_data cached_slabs[3]; // Cached slabs (0-3)
};

struct slab_cache_data
{
    struct slab_group *slab_group; // Slab group
    struct slab_meta *slab_meta; // Slab meta
    uint64_t free_bit_index; // Free bit index
};

#endif /* CACHE_H */
