#include "slab.h"

// ! TO TEST
struct slab_data *slab_data_addr_from_meta(struct slab_meta *slab_meta,
                                           size_t index)
{
    if (!slab_meta || index >= slab_meta->slab_used_len)
        return NULL;
    return (struct slab_data *)((char *)slab_meta->slabs_data
                                + get_slab_size(slab_meta) * index);
}

// ! TO TEST
void slab_data_init(struct slab_meta *slab_meta, size_t index)
{
    // TODO
    (void)slab_meta;
    (void)index;
}
