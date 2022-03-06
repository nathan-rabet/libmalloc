#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cache.h"
#include "slab.h"

int main(void)
{
    struct slab_group *group = slab_group_create(0, NULL);
    group = slab_group_create(1, group);
    group = slab_group_create(2, group);
    group = slab_group_create(3, group);

    struct slab_group *found = slab_group_find_enough_space(group, 4);

    // Bypass cache
    for (size_t i = 0; i < NB_CACHED_ENTRY; i++)
        slab_group_allocate(found, true);

    for (size_t i = 0; i < MAX_META_SLAB_USED / 2; i++)
        found->slabs_meta->slab_allocated[i] = true;

    for (size_t i = MAX_META_SLAB_USED / 2; i < MAX_META_SLAB_USED - 1; i++)
        found->slabs_meta->slab_dirty[i] = true;

    bool *ptr = slab_group_allocate(found, true);
    struct slab_meta *meta = page_begin(ptr);
    size_t index = slab_meta_retreive_index(ptr);

    (void)ptr;
    (void)meta;
    (void)index;
    return 0;
}
