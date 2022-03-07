#include <stdio.h>
#include <stdlib.h>

void malloc_then_free_then_malloc(void)
{
    void *p = malloc(1);
    free(p);
    p = malloc(2);
}

void basic1(void)
{
    int *p = malloc(12 * sizeof(int));

    for (size_t i = 0; i < 12; i++)
        p[i] = i;

    free(p);
}

void basic2()
{
    int *p = malloc(12 * sizeof(int));

    for (size_t i = 0; i < 12; i++)
        p[i] = i;

    p = realloc(p, 24 * sizeof(int));

    for (size_t i = 12; i < 24; i++)
        p[i] = i;

    for (size_t i = 0; i < 24; i++)
        printf("%d\n", p[i]);

    free(p);
}
