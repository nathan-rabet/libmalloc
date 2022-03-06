#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cache.h"
#include "slab.h"

int main(void)
{
    struct slab_cache cache = { 0 };
    struct slab_group slab_group = { .size_multiplicity = 42 };
    struct slab_meta slab_meta = { .nb_used_slabs = 24 };

    cache_add_data(&cache, &slab_group, &slab_meta, 1);
    cache_add_data(&cache, &slab_group, &slab_meta, 2);
    cache_add_data(&cache, &slab_group, &slab_meta, 3);

    cache_delete_by_index(&cache, 2);

    return 0;
}
