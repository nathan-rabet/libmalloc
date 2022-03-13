#include "debug.h"

#include "cast.h"
#include "maths.h"
#include "page.h"

bool debug_check_slab_data_access(struct slab_data *slab_data, size_t size,
                                  bool is_virgin)
{
    for (size_t i = 0; i < size; i++)
        if (slab_data->data[i] != 0 && is_virgin)
            return false;

    return true;
}

bool debug_calloc_slab_data_check_validity(struct slab_data *slab_data_old,
                                           size_t old_size,
                                           struct slab_data *slab_data_new,
                                           size_t new_size)
{
    // Check if the copy is valid.
    size_t i = 0;
    for (; i < MIN(old_size, new_size); i++)
        if (slab_data_new->data[i] != slab_data_old->data[i])
            return false;

    // Check accessibility of the rest of the slab_data.
    for (; i < new_size; i++)
    {
        byte_t a = slab_data_new->data[i];
        cast_ptr_to_size_t(&a);
    }

    return true;
}
