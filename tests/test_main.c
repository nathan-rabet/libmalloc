#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cache.h"
#include "slab.h"

int main(void)
{
    struct slab_group *slab_group = slab_group_create(2, NULL);

    for (size_t i = 0; i < 50 * MAX_META_SLAB_USED; i++)
        slab_meta_allocate(slab_group->slabs_meta, false);

    slab_meta_destroy_all(slab_group->slabs_meta);
    slab_group_destroy_all();

    return 0;
}
