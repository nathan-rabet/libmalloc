#include <stddef.h>
#include <string.h>

#include "cast.h"
#include "maths.h"
#include "overflow.h"
#include "slab.h"

static struct slab_group *slab_group = NULL;

__attribute__((visibility("default"))) void *malloc(size_t size)
{
    if (size > 0)
    {
        if (!slab_group)
            slab_group = slab_group_create(log2ceil(size), NULL);

        struct slab_group *my_slab_group =
            slab_group_find_enough_space(slab_group, size);

        if (!my_slab_group)
        {
            slab_group = slab_group_create(log2ceil(size), slab_group);
            my_slab_group = slab_group_find_enough_space(slab_group, size);
            if (!my_slab_group)
                return NULL;
        }

        bool *slab_meta_indexed = slab_group_allocate(my_slab_group, false);
        if (!slab_meta_indexed)
            return NULL;

        struct slab_data *slab_data = slab_data_from_meta_index(
            page_begin(slab_meta_indexed),
            slab_meta_retreive_index(slab_meta_indexed));
        if (!slab_data)
            return NULL;

        return &slab_data->data;
    }

    return NULL;
}

__attribute__((visibility("default"))) void free(void *ptr)
{
    if (ptr)
    {
        char *p = cast_ptr(ptr);
        struct slab_data *slab_data = cast_ptr(p - SLAB_HEADER_DATA_SIZE);
        slab_data_free(slab_data);
    }
}

__attribute__((visibility("default"))) void *realloc(void *ptr, size_t size)
{
    if (!ptr)
        return malloc(size);

    if (size == 0)
    {
        free(ptr);
        return NULL;
    }

    char *p = cast_ptr(ptr);
    struct slab_data *slab_data = cast_ptr(p - SLAB_HEADER_DATA_SIZE);

    if (coin_coin(slab_data))
    {
        char *new_alloc = malloc(size);
        if (new_alloc)
        {
            struct slab_meta *slab_meta = page_begin(slab_data);
            size_t old_size =
                power_2(slab_meta->common_group->size_multiplicity);
            size_t copy_size = MIN(old_size, size);
            memcpy(new_alloc, ptr, copy_size);
            free(ptr);
        }

        return new_alloc;
    }

    return NULL;
}

__attribute__((visibility("default"))) void *calloc(size_t nmemb, size_t size)
{
    if (nmemb > 0 && size > 0)
    {
        size_t total_size = nmemb * size;

        if (!slab_group)
            slab_group = slab_group_create(log2ceil(total_size), NULL);

        struct slab_group *my_slab_group =
            slab_group_find_enough_space(slab_group, total_size);

        if (!my_slab_group)
        {
            slab_group = slab_group_create(log2ceil(total_size), slab_group);
            my_slab_group =
                slab_group_find_enough_space(slab_group, total_size);
            if (!my_slab_group)
                return NULL;
        }

        bool *slab_meta_indexed = slab_group_allocate(my_slab_group, true);
        if (!slab_meta_indexed)
            return NULL;

        struct slab_data *slab_data = slab_data_from_meta_index(
            page_begin(slab_meta_indexed),
            slab_meta_retreive_index(slab_meta_indexed));
        if (!slab_data)
            return NULL;

        if (ptr_addition_has_overflow(slab_data, nmemb, size))
        {
            slab_data_free(slab_data);
            return NULL;
        }

        return &slab_data->data;
    }

    return NULL;
}
