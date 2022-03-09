#ifndef CACHE_H
#define CACHE_H

#include <stdbool.h>
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
    bool is_dirty; // Is not virgin (calloc friendly)
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
 * @param is_dirty Is the slab dirty.
 */
void cache_add_data(struct slab_cache *cache, struct slab_meta *slab_meta,
                    uint64_t free_bit_index, bool is_dirty);

/**
 * @brief Remove a cache entry by its index.
 *
 * @param cache The cache to remove from.
 * @param index The index of the cache entry to remove.
 */
void cache_delete_by_index(struct slab_cache *cache, uint8_t index);

/**
 * @brief Search a cache entry by its virginity.
 *
 * @param cache The cache to search in.
 * @param is_dirty Is the wished entry dirty or not.
 * @return int8_t The index of the entry if found, -1 otherwise.
 */
int8_t cache_find_by_virginity(struct slab_cache *cache, bool is_dirty);

/**
 * @brief Search a cache entry that must (or not) be virgin.
 *
 * This function can return a virgin entry even if `must_be_virgin` is false.
 *
 * @param cache The cache to search in.
 * @param must_be_virgin Is the wished entry virgin or not.
 * @return int8_t The index of the entry if found, -1 otherwise.
 */
int8_t cache_find_must_be_virgin(struct slab_cache *cache, bool must_be_virgin);

/**
 * @brief Delete all cache entries wich have the same slab meta.
 *
 * @param cache The cache to delete from.
 * @param slab_meta The meta address to search in.
 */
void cache_delete_all_occ_meta(struct slab_cache *cache,
                               struct slab_meta *slab_meta);

#endif /* CACHE_H */
