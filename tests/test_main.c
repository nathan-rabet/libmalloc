#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "slab.h"

int main(void)
{
    struct slab_group *slab_group = slab_group_create(2, NULL);

    slab_group->slabs_meta =
        slab_meta_create(slab_group->slabs_meta, slab_group);

    for (size_t i = 0; i < MAX_META_SLAB_USED; i++)
    {
        slab_data_init(slab_group->slabs_meta, i);
        printf("%zu\n", i);
    }

    slab_group_destroy_all(slab_group);
}
