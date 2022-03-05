#include <stddef.h>
#include <stdint.h>

size_t power_2(uint8_t multiplicity)
{
    return (1 << multiplicity);
}

size_t log_2(size_t value)
{
    if (value == 0)
        return 0;

    return 64 - __builtin_clzl(value) - 1;
}
