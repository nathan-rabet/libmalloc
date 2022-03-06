#include <stdio.h>
#include <stdlib.h>

int main(void)
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

    return 0;
}
