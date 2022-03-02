#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "slab.h"

int main(void)
{
    struct slab_group *slab_group = slab_group_create(
        floor(log2(128 * LOGARITHMIC_DECREASE_BYTES_THRESHOLD)), NULL);

    if (slab_group->slabs_meta->slab_used_len != 1)
    {
        printf("Slab used len is %ld\n", slab_group->slabs_meta->slab_used_len);
        exit(EXIT_FAILURE);
    }

    slab_group_destroy_all(slab_group);
    return 0;
}
