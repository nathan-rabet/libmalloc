#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bit.h"

int main(void)
{
    byte_t buffer = 0b00000001;

    get_bit(&buffer, 7);

    return 0;
}
