#ifndef PAGE_H
#define PAGE_H

#include <stddef.h>
#include <unistd.h>

/**
 * @brief Fit the given size to page ceil.
 *
 * @param n The size to fit.
 * @return The ceiled size.
 */
static inline size_t ceil_to_page(size_t n)
{
    return n % sysconf(_SC_PAGE_SIZE) == 0
        ? n
        : n + (sysconf(_SC_PAGE_SIZE) - (n % sysconf(_SC_PAGE_SIZE)));
}

/**
 * @brief Fit the given size to page floor.
 *
 * @param n The size to fit.
 * @return The floored size.
 */
static inline size_t floor_to_page(size_t n)
{
    return n & (~(sysconf(_SC_PAGE_SIZE) - 1));
}

/**
 * @brief Fit the given address to page begin.
 *
 * @param ptr The address to fit.
 * @return void* The floored address.
 */
static inline void *page_begin(void *ptr)
{
    return (void *)((size_t)ptr & (~(sysconf(_SC_PAGE_SIZE) - 1)));
}

#endif /* PAGE_H */
