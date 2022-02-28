#include <stddef.h>

__attribute__((visibility("default")))
void *malloc(size_t size)
{
    (void)size;
    return NULL;
}

__attribute__((visibility("default")))
void free(void *ptr)
{
    (void)ptr;
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
