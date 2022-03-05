#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "slab.h"

int main(void)
{
    double value = 0.0;
    for (size_t i = 0; i < 100; i++)
    {
        floor(log2(value));
        value += 1;
    }
}
