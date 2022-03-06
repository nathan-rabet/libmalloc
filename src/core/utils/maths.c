#include <stddef.h>
#include <stdint.h>

size_t power_2(uint8_t multiplicity)
{
    return (1 << multiplicity);
}

size_t log2ceil(size_t value)
{
    size_t log = 64 - __builtin_clzl(value) - 1;
    return log + (value > power_2(log));
}
