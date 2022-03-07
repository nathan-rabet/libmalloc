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

    for (size_t i = 0; i < MAX_META_SLAB_USED + 1; i++)
        slab_meta_allocate(slab_group->slabs_meta, false);

    bool isFree = slab_meta_free(slab_group->slabs_meta, 0);
    (void)isFree;

    return 0;
}
