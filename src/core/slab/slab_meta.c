#include <sys/mman.h>

#include "cast.h"
#include "maths.h"
#include "slab.h"

size_t get_slab_raw_size(struct slab_meta *slabs_meta)
{
    return power_2(slabs_meta->common_group->size_multiplicity);
}

size_t get_slab_size(struct slab_meta *slabs_meta)
{
    return SLAB_HEADER_DATA_SIZE + get_slab_raw_size(slabs_meta);
}

size_t get_meta_size(struct slab_meta *slabs_meta)
{
    return slabs_meta->slab_used_len * get_slab_size(slabs_meta);
}

struct slab_meta *slab_meta_create(struct slab_meta *linked_slab_meta,
                                   struct slab_group *common_group)
{
    if (!common_group)
        return NULL;

    struct slab_meta *new_slab_meta =
        mmap(NULL, SLAB_HEADER_META_SIZE, PROT_READ | PROT_WRITE,
             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (new_slab_meta == NULL)
        return NULL;

    // Add to head of linked list
    // (better for allocations, not for desallocations)
    new_slab_meta->next = linked_slab_meta;
    new_slab_meta->prev = NULL;
    new_slab_meta->common_group = common_group;
    if (linked_slab_meta)
        linked_slab_meta->prev = new_slab_meta;

    // Implies large values for small slabs (optimization)
    new_slab_meta->slab_used_len = MAX_META_SLAB_USED;
    size_t slab_size = get_meta_size(new_slab_meta);

    if (slab_size > LOGARITHMIC_DECREASE_BYTES_THRESHOLD)
    {
        int slab_size_log =
            __builtin_ctzl(2 * slab_size / LOGARITHMIC_DECREASE_BYTES_THRESHOLD);
        if (MAX_META_SLAB_USED - slab_size_log > 1)
            new_slab_meta->slab_used_len = MAX_META_SLAB_USED - slab_size_log;
        else
            new_slab_meta->slab_used_len = 1;
    }

    // Allocate the slabs
    new_slab_meta->slabs_data = mmap(NULL, slab_size, PROT_READ | PROT_WRITE,
                                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (new_slab_meta->slabs_data == NULL)
    {
        // Free the new slab meta
        if (munmap(new_slab_meta, SLAB_HEADER_META_SIZE) == -1)
            return NULL;
        return NULL;
    }

    return new_slab_meta;
}

// ! TO TEST
struct slab_meta *slab_meta_delete(struct slab_meta *slab_meta)
{
    if (slab_meta == NULL)
        return NULL;

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

    if (returned_slab_meta)
        while (returned_slab_meta->prev)
            returned_slab_meta = returned_slab_meta->prev;

    // TODO : Delete the slab group if no more meta

    // Delete slab_meta & the corresponding slab_data's
    if (munmap(slab_meta->slabs_data, get_meta_size(slab_meta)) == -1
        || munmap(slab_meta, sizeof(struct slab_meta)) == -1)
        return NULL;

    return returned_slab_meta;
}

size_t slab_meta_retreive_index(bool *slabs_meta)
{
    if (slabs_meta == NULL)
        return 0;
    struct slab_meta *slabs_meta_base = page_begin(slabs_meta);
    return cast_ptr_to_size_t(slabs_meta)
        - cast_ptr_to_size_t(slabs_meta_base->slab_allocated);
}

bool *slab_meta_allocate(struct slab_meta *slab_meta, bool must_be_virgin)
{
    if (slab_meta)
    {
        struct slab_meta *meta_to_allocate = NULL;
        ssize_t index_to_allocate = -1;

        // Cache usage
        if ((index_to_allocate = cache_find_by_virginity(
                 &slab_meta->common_group->cache, must_be_virgin))
            != -1)
        {
            meta_to_allocate =
                slab_meta->common_group->cache.cached_slabs[0].slab_meta;
            index_to_allocate =
                slab_meta->common_group->cache.cached_slabs[0].free_bit_index;
        }
        else
        {
            for (size_t i = 0; i < slab_meta->slab_used_len; i++)
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
            meta_to_allocate->nb_used_slabs++;
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
    if (!slab_meta || index >= slab_meta->slab_used_len
        || slab_meta->slab_allocated[index] == false)
        return false;

    slab_meta->nb_used_slabs--;
    cache_add_data(&slab_meta->common_group->cache, slab_meta, index, false);

    // TODO : Delete the slab group if no more meta
    if (slab_meta->nb_used_slabs == 0 && (slab_meta->prev || slab_meta->next))
        slab_meta->common_group->slabs_meta = slab_meta_delete(slab_meta);

    else
        slab_meta->slab_allocated[index] = false;

    return true;
}
