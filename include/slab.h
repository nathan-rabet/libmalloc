#ifndef SLAB_GROUP_H
#define SLAB_GROUP_H

#include <stdbool.h>
#include <stdint.h>

#include "bit.h"
#include "cache.h"
#include "page.h"

#define CANARY_MAGIC 0xFCC01C01 // sounds like "FC coin coin"
#define CANARY_HEAD_FUNCTION(meta_addr) (CANARY_MAGIC + 2 * (meta_addr))
#define CANARY_TAIL_FUNCTION(meta_addr) (CANARY_MAGIC - 2 * (meta_addr))

#define MAX_META_SLAB_USED 8
#define LOGARITHMIC_DECREASE_BYTES_THRESHOLD 2048

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

#define SLAB_HEADER_META_SIZE sizeof(struct slab_meta)
/**
 * @brief A slab_meta gives information about a set of slabs.
 *
 */
struct slab_meta
{
    struct slab_group
        *common_group; // Common slab group of the linked slabs meta
    struct slab_meta *prev; // Previous slab meta
    struct slab_meta *next; // Next slab meta
    struct slab_data *slabs_data; // Slabs RAW data (header + data)
    size_t nb_used_slabs; // Number of non-free slabs
    size_t slab_used_len; // Number of maximum handled slabs in the slab meta
                          // must be <= MAX_META_SLAB_USED
    bool slab_allocated[MAX_META_SLAB_USED]; // Are slabs used?
    bool slab_dirty[MAX_META_SLAB_USED]; // Have slabs allready been written ?
                                         // Used for calloc()
};

#define SLAB_HEADER_DATA_SIZE (sizeof(byte_t *) + 2 * sizeof(uint64_t))
/**
 * @brief A slab_data is where the user data is stored.
 *
 * All the allocation functions will return the address of a data[].
 * (and not the address of slab_data)
 */
struct slab_data
{
    uint64_t canary_tail; // Tail canary (to detect foward memory corruption).
    bool *my_meta_with_offset; // Meta slab + index of the slab meta.
    uint64_t canary_head; // Head canary (to detect backward memory corruption).
    byte_t data[]; // Slab raw data (user data).
};

// ----------------------------------------------------------------------------
// ? Slab group
// ----------------------------------------------------------------------------

/**
 * @brief Allocate a slab group of the given size multiplicity
 * + allocate a meta slab.
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
 * @brief Delete a slab group and refactor the linked list of slab groups.
 *
 * @param slab_group The slab group to destroy.
 *
 * @return struct slab_group* The new slab group head, or NULL in case of error.
 */
struct slab_group *slab_group_delete(struct slab_group *slab_group);

/**
 * @brief Destroy all the slab groups.
 *
 * @param slab_group The head of the linked list of slab groups.
 */
void slab_group_destroy_all(struct slab_group *slab_group);

/**
 * @brief Get the slab group with the multiplicity ceil(log2(size)).
 *
 * @param slab_group The head of the linked list of slab groups.
 * @param size The size of the slab group to get.
 * @return struct slab_group* The slab group with the multiplicity
 * ceil(log2(size)), or NULL if the slab group doesn't exists.
 */
struct slab_group *slab_group_find_enough_space(struct slab_group *slab_group,
                                                size_t size);

/**
 * @brief Find a free slab in the given slab group and allocate it.
 * If no free slab is found, allocate a new slab meta in the slab group.
 *
 * @param slab_group The slab group to allocate from.
 * @param must_be_virgin If true, the slab must be virgin. (calloc frendly)
 * @return bool* The slab_meta* + index of the slab.
 * The index can be retrieved with slab_meta_retreive_index().
 */
bool *slab_group_allocate(struct slab_group *slab_group, bool must_be_virgin);

// ----------------------------------------------------------------------------
// ? Slab meta
// ----------------------------------------------------------------------------

/**
 * @brief Get the length of the user-usable field of a slab
 * (header not included) maintained by a slab meta.
 *
 * @param slabs_meta
 * @return size_t The user-usable length of a single slab.
 */
size_t get_slab_raw_size(struct slab_meta *slabs_meta);

/**
 * @brief Get the length of a slab (header + data)
 * maintained by a slab meta.
 *
 * @param slabs_meta
 * @return size_t The length of a single slab.
 */
size_t get_slab_size(struct slab_meta *slabs_meta);

/**
 * @brief Get the total length of the slabs (header + data)
 * maintained by a slab meta.
 *
 * @param slabs_meta The slab meta to get the total length of.
 * @return size_t The total length of the slabs.
 */
size_t get_meta_size(struct slab_meta *slabs_meta);

/**
 * @brief Allocate a slab meta page.
 *
 * @param linked_slab_meta The actual linked list of slab meta.
 * @param common_group The common slab group of the slab meta.
 * @return struct slab_meta* The new slab meta head, or NULL in case of
 * error.
 */
struct slab_meta *slab_meta_create(struct slab_meta *linked_slab_meta,
                                   struct slab_group *common_group);
/**
 * @brief Delete a slab meta page.
 *
 * @param slab_meta The slab meta to destroy.
 * @return struct slab_meta* The new slab meta head, or NULL in case of
 * error.
 */
struct slab_meta *slab_meta_delete(struct slab_meta *slab_meta);

/**
 * @brief Retreive an index of a free slab in a slab meta.
 *
 * @param slabs_meta The slab meta to search in.
 * @return size_t The index of the free slab in the slab_meta.
 */
size_t slab_meta_retreive_index(bool *slabs_meta);

/**
 * @brief Reserve an available slab page in the provided slab meta or
 * create a new one if there is no available slab page.
 *
 * @param slab_meta The slab meta to allocate a slab in.
 * @param must_be_virgin If true, the slab must be virgin. (calloc frendly)
 * @return bool* The slab_meta* + index of the slab.
 * The index can be retrieved with slab_meta_retreive_index().
 */
bool *slab_meta_allocate(struct slab_meta *slab_meta, bool must_be_virgin);

/**
 * @brief Free a slab in a slab meta.
 *
 * @param slab_meta The slab meta to free.
 * @param index The index of the slab to free.
 * @return true If the slab was freed, false otherwise.
 */
bool slab_meta_free(struct slab_meta *slab_meta, size_t index);

// ----------------------------------------------------------------------------
// ? Slab data
// ----------------------------------------------------------------------------

/**
 * @brief Return &slabs_data[index] equivalent.
 *
 * @param slab_meta The slab meta to get the slab data from.
 * @param index The index where the slab data is.
 * @return struct slab_data*
 */
struct slab_data *slab_data_from_meta_index(struct slab_meta *slab_meta,
                                            size_t index);

/**
 * @brief Initialize a slab header data (canary, slab_meta).
 *
 * @param slab_meta The slab meta to initialize.
 * @param index The index of the slab to initialize.
 */
void slab_data_init(struct slab_meta *slab_meta, size_t index);

/**
 * @brief Get the slab data header object
 *
 * @param user_data The user data to get the slab data header from.
 * @param index The index in the slab_meta (returned).
 * @return struct slab_data* The slab data header.
 */
struct slab_data *get_slab_data_header(byte_t *user_data, size_t *index);

/**
 * @brief Verify a slab header data validity (canary, slab_meta).
 *
 * @note A slab_data has 2 canaries, one at the beginning and one at the end
 * of the slab_meta address.
 *
 * The rule is the following:
 * - canary_head = CANARY_MAGIC + 2 * my_meta_with_offset
 * - canary_tail = CANARY_MAGIC - 2 * my_meta_with_offset
 *
 * @param slab_data The slab data to verify.
 * @return true if the slab is valid, false otherwise.
 */
bool coin_coin(struct slab_data *slab_data);

/**
 * @brief Free a slab in its corresponding slab meta.
 *
 * @param slab_data The slab data to free.
 * @return true If the slab was freed, false otherwise.
 */
bool slab_data_free(struct slab_data *slab_data);

#endif /* SLAB_GROUP_H */
