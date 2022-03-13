#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "cast.h"
#include "debug.h"
#include "maths.h"
#include "overflow.h"
#include "slab.h"

__attribute__((visibility("default"))) void *malloc(size_t size)
{
    if (size > 0)
    {
        if (!slab_groups_head)
            slab_groups_head = slab_group_create(log2ceil(size), NULL);

        struct slab_group *my_slab_group =
            slab_group_find_enough_space(slab_groups_head, size);

        if (!my_slab_group)
        {
            slab_groups_head =
                slab_group_create(log2ceil(size), slab_groups_head);
            my_slab_group =
                slab_group_find_enough_space(slab_groups_head, size);
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

#ifdef DEBUG
        assert(debug_check_slab_data_access(slab_data, size, false));
#endif

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

__attribute__((visibility("default"))) void *calloc(size_t nmemb, size_t size)
{
    if (nmemb > 0 && size > 0)
    {
        size_t total_size = nmemb * size;

        if (!slab_groups_head)
            slab_groups_head = slab_group_create(log2ceil(total_size), NULL);

        struct slab_group *my_slab_group =
            slab_group_find_enough_space(slab_groups_head, total_size);

        if (!my_slab_group)
        {
            slab_groups_head =
                slab_group_create(log2ceil(total_size), slab_groups_head);
            my_slab_group =
                slab_group_find_enough_space(slab_groups_head, total_size);
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

#ifdef DEBUG
        assert(debug_check_slab_data_access(slab_data, total_size, true));
#endif

        return &slab_data->data;
    }

    return NULL;
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
        struct slab_meta *old_slab_meta =
            page_begin(slab_data->my_meta_with_offset);

        if (size <= get_slab_raw_size(old_slab_meta))
            return ptr;

        char *new_ptr = malloc(size);
        if (new_ptr)
        {
            size_t old_size = get_slab_raw_size(old_slab_meta);
            size_t copy_size = MIN(old_size, size);
            memcpy(new_ptr, ptr, copy_size);

#ifdef DEBUG
            struct slab_data *new_slab_data =
                cast_ptr(new_ptr - SLAB_HEADER_DATA_SIZE);

            assert(debug_calloc_slab_data_check_validity(slab_data, old_size,
                                                         new_slab_data, size));
#endif

            free(ptr);
        }

        return new_ptr;
    }

    return NULL;
}
