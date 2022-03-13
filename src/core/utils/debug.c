#include "debug.h"

#include "page.h"

bool debug_check_slab_data_access(struct slab_data *slab_data, bool is_virgin)
{
    size_t slab_size =
        get_slab_raw_size(page_begin(slab_data->my_meta_with_offset));

    for (size_t i = 0; i < slab_size; i++)
        if (slab_data->data[i] != 0 && is_virgin)
            return false;

    return true;
}
