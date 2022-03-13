#include "cast.h"

void *cast_ptr(void *ptr)
{
    return ptr;
}

size_t cast_ptr_to_size_t(void *ptr)
{
    return (size_t)ptr;
}

uint32_t cast_reduce_64bits_to_32bits(uint64_t a)
{
    return (uint32_t)a;
}
