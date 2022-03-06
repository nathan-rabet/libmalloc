#include <sys/mman.h>

#include "maths.h"
#include "slab.h"

struct slab_group *slab_group_create(uint8_t size_multiplicity,
                                     struct slab_group *linked_slab_group)

{
    struct slab_group *new_slab_group =
        mmap(NULL, sizeof(struct slab_group), PROT_READ | PROT_WRITE,
             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (new_slab_group == NULL)
        return NULL;

    new_slab_group->size_multiplicity = size_multiplicity;
    new_slab_group->slabs_meta = slab_meta_create(NULL, new_slab_group);

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
struct slab_group *slab_group_delete(struct slab_group *slab_group)
{
    if (slab_group == NULL)
        return NULL;

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

    if (returned_slab_group)
        while (returned_slab_group->prev)
            returned_slab_group = returned_slab_group->prev;

    // ! FIXME : Inccorect meta deletion
    if ((slab_group->slabs_meta
         && munmap(slab_group->slabs_meta,
                   get_meta_size(slab_group->slabs_meta))
             == -1)
        || munmap(slab_group, sizeof(struct slab_group)) == -1)
        return NULL;

    return returned_slab_group;
}

void slab_group_destroy_all(struct slab_group *slab_group)
{
    while (slab_group != NULL)
        slab_group = slab_group_delete(slab_group);
}

struct slab_group *slab_group_find_enough_space(struct slab_group *slab_group,
                                                size_t size)
{
    size_t logarithm = log2ceil(size);
    while (slab_group && slab_group->size_multiplicity != logarithm)
        slab_group = slab_group->next;

    return slab_group;
}

bool *slab_group_allocate(struct slab_group *slab_group, bool must_be_virgin)
{
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

    return slab_meta_allocate(slab_group->slabs_meta, must_be_virgin);
}
