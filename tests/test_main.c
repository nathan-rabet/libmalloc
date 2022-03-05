#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "slab.h"

int main(void)
{
    for (size_t i = 0; i < MAX_META_SLAB_USED + 1; i++)
    {
        struct slab_group *slab_group =
            slab_group_create(floor(log2(LOGARITHMIC_DECREASE_BYTES_THRESHOLD
                                         / MAX_META_SLAB_USED))
                                  + i,
                              NULL);

        slab_group_destroy_all(slab_group);
    }
    return 0;
}
