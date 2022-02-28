#ifndef SLAB_GROUP_H
#define SLAB_GROUP_H

#include <stdbool.h>
#include <stdint.h>

#include "bit.h"
#include "cache.h"

#    define CANARY_MAGIC 0xFCC01C01 // sounds like "FC coin coin"

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
    size_t used_count; // Number of used slabs
    size_t slab_used_len; // Number of used slabs
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

/**
 * @brief Allocate a slab group of the given size multiplicity.
 *
 * @note The linked list of slab groups is sorted by size multiplicity.
 *
 * @param size_multiplicity The size multiplicity of the slab group to allocate.
 * @param linked_slab_group The actual linked list of slab groups.
 * @return struct slab_group* The new slab group head, or NULL in case of error.
 */
struct slab_group *slab_group_create(uint8_t size_multiplicity,
                                     struct slab_group *linked_slab_group);

/**
 * @brief Destroy a slab group.
 *
 * @param slab_group The slab group to destroy.
 *
 * @return struct slab_group* The new slab group head, or NULL in case of error.
 */
struct slab_group *slab_group_destroy(struct slab_group *slab_group);

/**
 * @brief Allocate (sorted) a slab meta page.
 *
 * @param linked_slab_meta The actual linked list of slab meta.
 * @return struct slab_meta* The new slab meta head, or NULL in case of error.
 */
struct slab_meta *slab_meta_create(struct slab_meta *linked_slab_meta);

/**
 * @brief Destroy a slab meta page.
 *
 * @param slab_meta The slab meta to destroy.
 * @return struct slab_meta* The new slab meta head, or NULL in case of error.
 */
struct slab_meta *slab_meta_destroy(struct slab_meta *slab_meta);

/**
 * @brief Use a free space in a slab meta.
 *
 * @param slab_meta The slab meta to use.
 * @return struct slab_data* The address of the user-space struct slab_data *,
 * or NULL in case of error.
 */
struct slab_data *slab_meta_use_one(struct slab_meta *slab_meta);

/**
 * @brief Free a slab in a slab meta.
 *
 * @param slab_meta The slab meta to free.
 * @param index The index of the slab to free.
 * @return true If the slab was freed, false otherwise.
 */
bool slab_meta_free(struct slab_meta *slab_meta, size_t index);

/**
 * @brief Initialize a slab header data (canary, slab_meta).
 *
 * @param slab_meta The slab meta to initialize.
 * @return struct slab_data*
 */
struct slab_data *slab_data_init(struct slab_meta *slab_meta);

/**
 * @brief Free a slab in its corresponding slab meta.
 *
 * @param slab_data The slab data to free.
 * @return true If the slab was freed, false otherwise.
 */
bool slab_data_free(struct slab_data *slab_data);

/**
 * @brief Verify a slab header data validity (canary, slab_meta).
 *
 * @param slab_data The slab data to verify.
 * @return true if the slab is valid, false otherwise.
 */
bool coin_coin(struct slab_data *slab_data);

/**
 * @brief Get the slab data header object
 *
 * @param user_data The user data to get the slab data header from.
 * @return struct slab_data* The slab data header.
 */
struct slab_data *get_slab_data_header(byte_t *user_data);
#endif /* SLAB_GROUP_H */
