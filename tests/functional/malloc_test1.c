#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int *p = malloc(12 * sizeof(int));

    for (size_t i = 0; i < 12; i++)
        p[i] = i;

    return 0;
}
