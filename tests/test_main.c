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

    size_t slab_size = get_slab_raw_size(slab_group->slabs_meta);

    return 0;
}
