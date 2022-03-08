#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SLAB_HEADER_DATA_SIZE (sizeof(bool *) + 2 * sizeof(uint64_t))

void malloc_then_free_then_malloc(void)
{
    void *p = malloc(1);
    free(p);
    p = malloc(2);
}

void heap_fucker(void)
{
    void *p[100];
    unsigned long i, j;
    for (j = 0; j < 42; j++)
    {
        for (i = 0; i < sizeof p / sizeof *p; i++)
        {
            p[i] = malloc(4000);
            memset((char *)p[i] - SLAB_HEADER_DATA_SIZE, 0, 4000);
        }
        for (i = 0; i < sizeof p / sizeof *p; i++)
            if (i % 150)
                free(p[i]);
        for (i = 0; i < sizeof p / sizeof *p; i += 150)
            free(p[i]);
    }
}

void basic1(void)
{
    unsigned long *p = malloc(12 * sizeof(int));

    for (size_t i = 0; i < 12; i++)
        p[i] = i;

    free(p);
}

void basic2()
{
    unsigned long *p = malloc(12 * sizeof(int));

    for (size_t i = 0; i < 12; i++)
        p[i] = i;

    p = realloc(p, 24 * sizeof(int));

    for (size_t i = 12; i < 24; i++)
        p[i] = i;

    free(p);
}
