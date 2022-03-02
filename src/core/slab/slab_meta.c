#include <sys/mman.h>

#include "maths.h"
#include "slab.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

size_t get_meta_size(struct slab_meta *slabs_meta)
{
    return slabs_meta->slab_used_len
        * (SLAB_HEADER_DATA_SIZE
           + power2(slabs_meta->common_group->size_multiplicity));
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
        int slab_size_log = __builtin_ctzl(LOGARITHMIC_DECREASE_BYTES_THRESHOLD)
            - __builtin_ctzl(slab_size);
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
        if (munmap(new_slab_meta, SLAB_HEADER_META_SIZE) == -1)
            return NULL;
        return NULL;
    }

    return new_slab_meta;
}

// TODO : Code review
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

// TODO : Code review
bool slab_meta_free(struct slab_meta *slab_meta, size_t index)
{
    if (!slab_meta || index >= slab_meta->slab_used_len)
        return false;

    slab_meta->nb_used_slabs--;
    if (slab_meta->nb_used_slabs == 0)
        slab_meta->common_group->slabs_meta = slab_meta_delete(slab_meta);

    else
        slab_meta->slab_used[index] = false;

    return true;
}

size_t slab_meta_allocate(struct slab_meta *slab_meta)
{
    // TODO
    (void)slab_meta;
    return 0;
}
