#include <criterion/criterion.h>
#include <math.h>

#include "maths.h"
#include "slab.h"

Test(slab_meta_create, logarithmic_decreases)
{
    // ? THREAD UNSAFE FOR HIGH NUMBER `i`
    for (size_t i = 0; i < MAX_META_SLAB_USED + 10; i++)
    {
        struct slab_group *slab_group =
            slab_group_create(floor(log2(LOGARITHMIC_DECREASE_BYTES_THRESHOLD
                                         / MAX_META_SLAB_USED))
                                  + i,
                              NULL);

        cr_assert_eq(slab_group->slabs_meta->slab_used_len,
                     MAX(1, MAX_META_SLAB_USED - 1 - i), "Slab used len is %ld",
                     slab_group->slabs_meta->slab_used_len);
        slab_group_destroy_all(slab_group);
    }
}
