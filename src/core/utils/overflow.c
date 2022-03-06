#include "overflow.h"

#include "cast.h"

bool ptr_addition_has_overflow(void *ptr, size_t nmemb, size_t size)
{
    size_t total_elements;

    if (__builtin_umull_overflow(nmemb, size, &total_elements))
        return true;

    unsigned long lptr = cast_ptr_to_size_t(ptr);
    if (__builtin_uaddl_overflow(lptr, total_elements, &lptr))
        return true;

    return false;
}
