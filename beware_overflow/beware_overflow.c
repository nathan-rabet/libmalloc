

#include <stddef.h>

void *beware_overflow(void *ptr, size_t nmemb, size_t size)
{
    size_t total_elements;

    if (__builtin_umull_overflow(nmemb, size, &total_elements))
        return NULL;

    unsigned long lptr = ptr - NULL;
    if (__builtin_uaddl_overflow(lptr, total_elements, &lptr))
        return NULL;

    return ptr + total_elements;
}
