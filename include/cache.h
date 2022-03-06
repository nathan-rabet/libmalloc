#ifndef CACHE_H
#define CACHE_H

#include <stddef.h>
#include <stdint.h>

#define NB_CACHED_ENTRY 3

/**
 * @brief The data of a slab cache.
 *
 */
struct slab_cache_data
{
    struct slab_meta *slab_meta; // Slab meta
    size_t free_bit_index; // Free bit index
};

/**
 * @brief Cache used in a slab_group.
 *
 * It is used to store a free slab meta in order to access it faster.
 *
 */
struct slab_cache
{
    uint8_t nb_cached_slabs : 2; // Number of cached slabs (0-3)
    struct slab_cache_data cached_slabs[NB_CACHED_ENTRY]; // Cached slabs (0-3)
};

/**
 * @brief Add slab information to a cache.
 *
 * @param cache The cache to add the information to.
 * @param slab_meta The slab meta address to add.
 * @param free_bit_index The free bit index to add.
 */
void cache_add_data(struct slab_cache *cache, struct slab_meta *slab_meta,
                    uint64_t free_bit_index);

/**
 * @brief Remove a cache entry by its index.
 *
 * @param cache The cache to remove from.
 * @param index The index of the cache entry to remove.
 */
void cache_delete_by_index(struct slab_cache *cache, uint8_t index);

#endif /* CACHE_H */
