#include <stddef.h>

__attribute__((visibility("default"))) void *aligned_alloc(size_t alignment,
                                                           size_t size)
{
    return NULL;
}

__attribute__((visibility("default"))) size_t malloc_usable_size(void *ptr)
{}

__attribute__((visibility("default"))) void *memalign(size_t alignment,
                                                      size_t size)
{
    return NULL;
}

__attribute__((visibility("default"))) int
posix_memalign(void **memptr, size_t alignment, size_t size)
{
    return NULL;
}
