#include <assert.h>
#include <sys/mman.h>

#include "maths.h"
#include "slab.h"

struct slab_group *slab_group_create(uint8_t size_multiplicity,
                                     struct slab_group *linked_slab_group)

{
#ifdef DEBUG
#endif

    struct slab_group *new_slab_group =
        mmap(NULL, sizeof(struct slab_group), PROT_READ | PROT_WRITE,
             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (new_slab_group == MAP_FAILED)
        return NULL;

    new_slab_group->size_multiplicity = size_multiplicity;
    new_slab_group->slabs_meta = slab_meta_create(NULL, new_slab_group);

    if (!new_slab_group->slabs_meta)
    {
        munmap(new_slab_group, sizeof(struct slab_group));
        return NULL;
    }

    // Sort the linked list of slab groups by size_multiplicity
    if (linked_slab_group != NULL)
    {
        // If the new slab group is smaller than the first slab group
        if (size_multiplicity < linked_slab_group->size_multiplicity)
        {
            new_slab_group->next = linked_slab_group;
            new_slab_group->prev = NULL;
            linked_slab_group->prev = new_slab_group;
        }
        else
        {
            struct slab_group *current_slab_group = linked_slab_group;
            while (current_slab_group->next != NULL
                   && size_multiplicity
                       > current_slab_group->next->size_multiplicity)
                current_slab_group = current_slab_group->next;

            if (current_slab_group->next != NULL) // Not the last element
            {
                // Disallow duplicates
                if (size_multiplicity
                    == current_slab_group->next->size_multiplicity)
                {
                    if (munmap(new_slab_group->slabs_meta,
                               get_meta_size(new_slab_group->slabs_meta))
                            == -1
                        || munmap(new_slab_group, sizeof(struct slab_group))
                            == -1)
                        return NULL;
                    return linked_slab_group;
                }

                // current_slab_group <-> new_slab_group <->
                // current_slab_group->next
                new_slab_group->next = current_slab_group->next;
                new_slab_group->prev = current_slab_group;
                current_slab_group->next->prev = new_slab_group;
                current_slab_group->next = new_slab_group;
            }
            else // The last element
            {
                current_slab_group->next = new_slab_group;
                new_slab_group->prev = current_slab_group;
            }
            return linked_slab_group;
        }
    }

    return new_slab_group;
}

// ! TO TEST
// ! FIXME : Thread unsafe
bool slab_group_delete(struct slab_group *slab_group)
{
    if (slab_group == NULL)
        return (slab_groups_head = NULL), false;

    struct slab_group *next_slab_group = slab_group->next;
    struct slab_group *prev_slab_group = slab_group->prev;

    struct slab_group *returned_slab_group = NULL;
    if (prev_slab_group != NULL)
    {
        returned_slab_group = prev_slab_group;
        prev_slab_group->next = next_slab_group;
    }
    if (next_slab_group != NULL)
    {
        returned_slab_group = next_slab_group;
        next_slab_group->prev = prev_slab_group;
    }

    // TODO : O(1) operation ?
    if (returned_slab_group)
        while (returned_slab_group->prev)
            returned_slab_group = returned_slab_group->prev;

    // TODO Must abort if unsuccessful
    munmap(slab_group, sizeof(struct slab_group));
    return (slab_groups_head = returned_slab_group) != NULL;
}

void slab_group_destroy_all(void)
{
    while (slab_group_delete(slab_groups_head))
        ;
}

struct slab_group *slab_group_find_enough_space(struct slab_group *slab_group,
                                                size_t size)
{
#ifdef DEBUG
    assert(slab_group);
    assert(size > 0);
#endif

    size_t logarithm = log2ceil(size);
    while (slab_group && slab_group->size_multiplicity != logarithm)
        slab_group = slab_group->next;

    return slab_group;
}

bool *slab_group_allocate(struct slab_group *slab_group, bool must_be_virgin)
{
#ifdef DEBUG
    assert(slab_group);
#endif

    if (slab_group == NULL)
        return NULL;
    struct slab_meta *slab_meta = slab_group->slabs_meta;

    bool *allocation = NULL;
    while (slab_meta && !allocation)
    {
        allocation = slab_meta_allocate(slab_meta, must_be_virgin);
        slab_meta = slab_meta->next;
    }

    if (allocation)
        return allocation;

    // In case of a full slab_meta, allocate a new one
    slab_group->slabs_meta =
        slab_meta_create(slab_group->slabs_meta, slab_group);

    if (!slab_group->slabs_meta)
        return NULL;

    return slab_meta_allocate(slab_group->slabs_meta, must_be_virgin);
}
