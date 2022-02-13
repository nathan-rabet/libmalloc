#include "page_begin.h"

void *page_begin(void *ptr, size_t page_size)
{
    return (void *)((size_t)ptr & (~(page_size - 1)));
}
