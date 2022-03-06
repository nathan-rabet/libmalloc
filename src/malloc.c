#include <stddef.h>

#include "maths.h"
#include "slab.h"

static struct slab_group *slab_group = NULL;

__attribute__((visibility("default")))
void *malloc(size_t size)
{
    if (size > 0)
    {
        if (!slab_group)
            slab_group = slab_group_create(log2ceil(size), NULL);
    }

    return NULL;
}

__attribute__((visibility("default")))
void free(void *ptr)
{
    if (ptr)
    {
        char *p = cast_ptr(ptr);
        struct slab_data *slab_data = p - SLAB_HEADER_DATA_SIZE;
        slab_data_free(slab_data);
    }
}

__attribute__((visibility("default")))
void *realloc(void *ptr, size_t size)
{
    (void)ptr;
    (void)size;
    return NULL;
}

__attribute__((visibility("default")))
void *calloc(size_t nmemb, size_t size)
{
    (void)nmemb;
    (void)size;
    return NULL;
}
