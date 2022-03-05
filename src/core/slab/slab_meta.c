#include <sys/mman.h>

#include "maths.h"
#include "slab.h"

size_t get_slab_raw_size(struct slab_meta *slabs_meta)
{
    return power2(slabs_meta->common_group->size_multiplicity);
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

    if (prev_slab_meta != NULL)
        prev_slab_meta->next = next_slab_meta;
    if (next_slab_meta != NULL)
        next_slab_meta->prev = prev_slab_meta;

    // Delete slab_meta & the corresponding slab_data's
    if (munmap(slab_meta->slabs_data, get_meta_size(slab_meta)) == -1
        || munmap(slab_meta, sizeof(struct slab_meta)) == -1)
        return NULL;

    return next_slab_meta;
}

// ! TO TEST
size_t slab_meta_allocate(struct slab_meta *slab_meta)
{
    if (slab_meta)
    {
        for (size_t i = 0; i < slab_meta->slab_used_len; i++)
        {
            if (slab_meta->slab_used[i] == false)
            {
                slab_meta->slab_used[i] = true;
                slab_data_init(slab_meta, i);

                return i;
            }
        }
    }
    return -1;
}

bool slab_meta_free(struct slab_meta *slab_meta, size_t index)
{
    if (!slab_meta || index >= slab_meta->slab_used_len
        || slab_meta->slab_used[index] == false)
        return false;

    slab_meta->nb_used_slabs--;
    if (slab_meta->nb_used_slabs == 0)
        slab_meta->common_group->slabs_meta = slab_meta_delete(slab_meta);

    else
        slab_meta->slab_used[index] = false;

    return true;
}
