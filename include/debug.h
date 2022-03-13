
#ifndef DEBUG_H
#define DEBUG_H

#include <stdbool.h>

#include "slab.h"

bool debug_check_slab_data_access(struct slab_data *slab_data, bool is_virgin);

#endif // DEBUG_H
