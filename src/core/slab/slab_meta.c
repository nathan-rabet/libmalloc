#include <assert.h>
#include <sys/mman.h>

#include "cast.h"
#include "maths.h"
#include "slab.h"

#define MALLOC_ALIGNMENT 16

size_t get_slab_raw_size(struct slab_meta *slabs_meta)
{
#ifdef DEBUG
    assert(slabs_meta);
#endif
    return power_2(slabs_meta->common_group->size_multiplicity);
}

size_t get_slab_size(struct slab_meta *slabs_meta)
{
#ifdef DEBUG
    assert(slabs_meta);
#endif
    return SLAB_HEADER_DATA_SIZE + get_slab_raw_size(slabs_meta);
}

size_t get_meta_size(struct slab_meta *slabs_meta)
{
#ifdef DEBUG
    assert(slabs_meta);
#endif
    return slabs_meta->max_handled_slabs * get_slab_size(slabs_meta);
}

struct slab_meta *slab_meta_create(struct slab_meta *linked_slab_meta,
                                   struct slab_group *common_group)
{
#ifdef DEBUG
    assert(common_group);
#endif

    if (!common_group)
        return NULL;

    struct slab_meta *new_slab_meta =
        mmap(NULL, SLAB_HEADER_META_SIZE, PROT_READ | PROT_WRITE,
             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (new_slab_meta == MAP_FAILED)
        return NULL;

    // Add to head of linked list
    // (better for allocations, not for desallocations)
    new_slab_meta->next = linked_slab_meta;
    new_slab_meta->prev = NULL;
    new_slab_meta->common_group = common_group;
    if (linked_slab_meta)
        linked_slab_meta->prev = new_slab_meta;

    // Implies large values for small slabs (optimization)
    new_slab_meta->max_handled_slabs = MAX_META_SLAB_USED;
    size_t slab_size = get_meta_size(new_slab_meta);

    if (slab_size > LOGARITHMIC_DECREASE_BYTES_THRESHOLD)
    {
        int slab_size_log = __builtin_ctzl(
            2 * slab_size / LOGARITHMIC_DECREASE_BYTES_THRESHOLD);
        if (MAX_META_SLAB_USED - slab_size_log > 1)
            new_slab_meta->max_handled_slabs =
                MAX_META_SLAB_USED - slab_size_log;
        else
            new_slab_meta->max_handled_slabs = 1;
    }

    slab_size = get_meta_size(new_slab_meta);
    // Allocate the slabs
    new_slab_meta->slabs_data =
        mmap(NULL, slab_size + MALLOC_ALIGNMENT, PROT_READ | PROT_WRITE,
             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    new_slab_meta->slabs_data += MALLOC_ALIGNMENT
        - cast_ptr_to_size_t(new_slab_meta->slabs_data) % MALLOC_ALIGNMENT;

    if (new_slab_meta->slabs_data == MAP_FAILED)
    {
        // Free the new slab meta
        if (munmap(new_slab_meta, SLAB_HEADER_META_SIZE) == -1)
            return NULL;
        return NULL;
    }

    // Add some virgin slabs to the cache
    // (virgins are allways better than the dirty ones)
    // TODO : Better way to do this ?
    size_t nb_entry_to_add;
    if (new_slab_meta->max_handled_slabs > MAX_META_SLAB_USED)
        nb_entry_to_add = MAX_META_SLAB_USED;
    else
        nb_entry_to_add = new_slab_meta->max_handled_slabs;

    // Delete existing dirty slabs
    size_t entry_index = 0;
    for (size_t cache_i = 0;
         entry_index < nb_entry_to_add && cache_i < NB_CACHED_ENTRY; cache_i++)
    {
        if (cache_i == new_slab_meta->common_group->cache.nb_cached_slabs)
            cache_add_data(&new_slab_meta->common_group->cache, new_slab_meta,
                           entry_index++, false);

        else if (new_slab_meta->common_group->cache.cached_slabs[cache_i]
                     .is_dirty
                 == true)
        {
            cache_delete_by_index(&new_slab_meta->common_group->cache, cache_i);
            cache_add_data(&new_slab_meta->common_group->cache, new_slab_meta,
                           entry_index++, false);
        }
    }

    return new_slab_meta;
}

bool slab_meta_delete(struct slab_meta *slab_meta)
{
#ifdef DEBUG
    assert(slab_meta);
#endif

    if (!slab_meta)
        return false;

    struct slab_meta *next_slab_meta = slab_meta->next;
    struct slab_meta *prev_slab_meta = slab_meta->prev;

    struct slab_meta *returned_slab_meta = NULL;

    if (prev_slab_meta != NULL)
    {
        returned_slab_meta = prev_slab_meta;
        prev_slab_meta->next = next_slab_meta;
    }
    if (next_slab_meta != NULL)
    {
        returned_slab_meta = next_slab_meta;
        next_slab_meta->prev = prev_slab_meta;
    }

    // Find the head of the linked list
    // TODO : O(1) operation ?
    if (returned_slab_meta)
        while (returned_slab_meta->prev)
            returned_slab_meta = returned_slab_meta->prev;

    // Delete the cache entry containing the slab meta
    cache_delete_all_occ_meta(&slab_meta->common_group->cache, slab_meta);

    // Get the size of the slab meta.
    size_t slab_size = get_meta_size(slab_meta);

    if (!returned_slab_meta)
        // If there is no more slab meta, we can delete the slab group
        slab_group_delete(slab_meta->common_group);
    else
        // Update slab_meta->common_group->slabs_meta
        slab_meta->common_group->slabs_meta = returned_slab_meta;

    // Delete slab_meta & the corresponding slab_data's
    if (munmap(page_begin(slab_meta->slabs_data), slab_size) == -1
        || munmap(slab_meta, sizeof(struct slab_meta)) == -1)
        return NULL;

    return returned_slab_meta != NULL;
}

void slab_meta_destroy_all(struct slab_meta *slab_meta)
{
#ifdef DEBUG
    assert(slab_meta);
#endif

    struct slab_group *common_group = slab_meta->common_group;
    while (slab_meta_delete(common_group->slabs_meta))
        ;
}

size_t slab_meta_retreive_index(bool *slabs_meta)
{
#ifdef DEBUG
    assert(slabs_meta);
#endif
    // Protected by higher level functions
    // if (!slabs_meta)
    //     return 0;
    struct slab_meta *slabs_meta_base = page_begin(slabs_meta);
    return cast_ptr_to_size_t(slabs_meta)
        - cast_ptr_to_size_t(slabs_meta_base->slab_allocated);
}

bool *slab_meta_allocate(struct slab_meta *slab_meta, bool must_be_virgin)
{
#ifdef DEBUG
    assert(slab_meta);
#endif
    if (slab_meta)
    {
        // If the slab meta is full, we need to allocate a new one
        if (slab_meta->nb_allocated_slabs == slab_meta->max_handled_slabs)
        {
            slab_meta->common_group->slabs_meta = slab_meta_create(
                slab_meta->common_group->slabs_meta, slab_meta->common_group);

            if (slab_meta->common_group->slabs_meta == NULL)
                return NULL;

            return slab_meta_allocate(slab_meta->common_group->slabs_meta,
                                      must_be_virgin);
        }

        struct slab_meta *meta_to_allocate = NULL;
        ssize_t index_to_allocate = -1;
        int8_t cache_index = -1;

        // Cache usage
        if ((cache_index = cache_find_must_be_virgin(
                 &slab_meta->common_group->cache, must_be_virgin))
            != -1)
        {
            meta_to_allocate =
                slab_meta->common_group->cache.cached_slabs[cache_index]
                    .slab_meta;
            index_to_allocate =
                slab_meta->common_group->cache.cached_slabs[cache_index]
                    .free_bit_index;
            cache_delete_by_index(&slab_meta->common_group->cache, cache_index);
        }
        else
        {
            for (size_t i = 0; i < slab_meta->max_handled_slabs; i++)
            {
                if (slab_meta->slab_allocated[i] == false
                    && IMPLIES(must_be_virgin,
                               slab_meta->slab_dirty[i] == false))

                {
                    meta_to_allocate = slab_meta;
                    index_to_allocate = i;
                    break;
                }
            }
        }

        if (meta_to_allocate)
        {
            meta_to_allocate->nb_allocated_slabs++;
            meta_to_allocate->slab_allocated[index_to_allocate] = true;
            meta_to_allocate->slab_dirty[index_to_allocate] = true;
            slab_data_init(meta_to_allocate, index_to_allocate);

            return &meta_to_allocate->slab_allocated[index_to_allocate];
        }
    }

    return NULL;
}

bool slab_meta_free(struct slab_meta *slab_meta, size_t index)
{
#ifdef DEBUG
    assert(slab_meta && index < slab_meta->max_handled_slabs);
#endif
    if (!slab_meta || index >= slab_meta->max_handled_slabs
        || slab_meta->slab_allocated[index] == false)
        return false;

    slab_meta->nb_allocated_slabs--;
    if (slab_meta->nb_allocated_slabs == 0
        && (slab_meta->prev || slab_meta->next))
        slab_meta_delete(slab_meta);

    else
    {
        slab_meta->slab_allocated[index] = false;
        cache_add_data(&slab_meta->common_group->cache, slab_meta, index,
                       slab_meta->slab_dirty[index]);
    }

    return true;
}
