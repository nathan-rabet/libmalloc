#include "allocator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define MODP(X, Y) (((X) + (Y)) % (Y))

struct blk_allocator *blka_new(void)
{
    return calloc(1, sizeof(struct blk_allocator));
}

static size_t ceil_to_page(size_t n)
{
    return n + (sysconf(_SC_PAGE_SIZE) - MODP(n, sysconf(_SC_PAGE_SIZE)));
}

struct blk_meta *blka_alloc(struct blk_allocator *blka, size_t size)
{
    if (blka)
    {
        if (blka->meta)
            blka->meta->next = blka->meta;
        blka->meta = mmap(NULL, size, PROT_READ | PROT_WRITE,
                          MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

        blka->meta->size =
            ceil_to_page(size) - (sizeof(struct blk_meta *) + sizeof(size_t));

        return blka->meta;
    }

    return NULL;
}

void blka_free(struct blk_meta *blk)
{
    munmap(blk, sizeof(struct blk_meta *) + sizeof(size_t) + blk->size);
}

void blka_pop(struct blk_allocator *blka)
{
    if (blka && blka->meta)
    {
        struct blk_meta *tmp = blka->meta->next;

        blka_free(blka->meta);
        blka->meta = tmp;
    }
}

void blka_delete(struct blk_allocator *blka)
{
    if (blka)
    {
        while (blka->meta)
            blka_pop(blka);
        free(blka);
    }
}
