#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int *p = malloc(12);
    int *rp = realloc(p, 0);

    printf("realloc(p,0) = %p\n", rp);
}
