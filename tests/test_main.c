#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "slab.h"

int main(void)
{
    struct slab_group *group = slab_group_create(0, NULL);
    group = slab_group_create(1, group);
    group = slab_group_create(2, group);
    group = slab_group_create(3, group);

    // Delete group
    group->next->next = slab_group_delete(group->next->next);

    return 0;
}
