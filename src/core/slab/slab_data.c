#include <err.h>
#include <stddef.h>

#include "cast.h"
#include "slab.h"

struct slab_data *slab_data_from_meta_index(struct slab_meta *slab_meta,
                                            size_t index)
{
    if (!slab_meta || index >= slab_meta->max_handled_slabs)
        err(1,
            "slab_data_from_meta_index: invalid params, index: %zu but max: "
            "%zu",
            index, slab_meta->max_handled_slabs);
    char *slabs_data = cast_ptr(slab_meta->slabs_data);
    struct slab_data *slabs_data_addr =
        cast_ptr(slabs_data + get_slab_size(slab_meta) * index);

    return slabs_data_addr;
}

void slab_data_init(struct slab_meta *slab_meta, size_t index)
{
    if (slab_meta)
    {
        struct slab_data *slab_data =
            slab_data_from_meta_index(slab_meta, index);
        slab_data->my_meta_with_offset = slab_meta->slab_allocated + index;

        slab_data->canary_head = CANARY_HEAD_FUNCTION(
            cast_ptr_to_size_t(slab_data->my_meta_with_offset));

        slab_data->canary_tail = CANARY_TAIL_FUNCTION(
            cast_ptr_to_size_t(slab_data->my_meta_with_offset));
    }
}

bool coin_coin(struct slab_data *slab_data)
{
    return slab_data->canary_head
        == CANARY_HEAD_FUNCTION(
               cast_ptr_to_size_t(slab_data->my_meta_with_offset))
        && slab_data->canary_tail
        == CANARY_TAIL_FUNCTION(
               cast_ptr_to_size_t(slab_data->my_meta_with_offset));
}

bool slab_data_free(struct slab_data *slab_data)
{
    // |🦆| my_meta_with_offset |🦆|
    if (slab_data && coin_coin(slab_data))
    {
        struct slab_meta *meta = page_begin(slab_data->my_meta_with_offset);
        size_t index = slab_meta_retreive_index(slab_data->my_meta_with_offset);

        return slab_meta_free(meta, index);
    }
    return false;
}
