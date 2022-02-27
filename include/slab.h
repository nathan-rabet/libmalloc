#ifndef SLAB_GROUP_H
#define SLAB_GROUP_H

#include <stdbool.h>
#include <stdint.h>

#include "bit.h"
#include "cache.h"

#define byte_t unsigned char // used in `slab_data`
#define CANARY_MAGIC 0xFFC01C01 // "coin coin"

/**
 * @brief A slab_group is a collection of slabs with the same size.
 * The size of a slab group is a power of 2 (1, 2, 4, 8, 16, 32, 64, 128, ...)
 */
struct slab_group
{
    struct slab_group *prev; // Previous slab group
    struct slab_group *next; // Next slab group
    uint8_t size_multiplicity; // Group size = 2 ^ `size_multiplicity`
    struct slab_cache cache; // Slab cache
    struct slab_meta *slabs_meta; // Bitmap of free slabs
};

/**
 * @brief A slab_meta gives information about a set of slabs.
 *
 */
struct slab_meta
{
    struct slab_meta *prev; // Previous slab meta
    struct slab_meta *next; // Next slab meta
    struct slab_data *slabs_data; // Slabs data
    uint64_t used_count; // Number of used slabs
    byte_t slab_used[]; // Are slabs used? (0 = free, 1 = used)
                        // must be accessed with
                        // `get_bit_buffer` and `set_bit_buffer`
};

/**
 * @brief A slab_data is where the user data is stored.
 *
 * All the allocation functions will return the address of a data[].
 * (and not the address of slab_data)
 */
struct slab_data
{
    byte_t *meta_used; // Meta slab of the current slab_data
    uint64_t canary; // Canary (`my slab_meta addr` + 2 * canary = "coin coin")
    byte_t data[]; // Slab raw data (user data)
};

#endif /* SLAB_GROUP_H */
