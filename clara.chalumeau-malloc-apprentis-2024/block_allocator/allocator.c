#include "allocator.h"

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

struct blk_allocator *blka_new(void)
{
    struct blk_allocator *new_allocator = malloc(sizeof(struct blk_allocator));
    if (!new_allocator)
        err(1, "Impossible to malloc");
    new_allocator->meta = NULL;
    return new_allocator;
}
void blka_delete(struct blk_allocator *blka)
{
    struct blk_meta *blk_meta = blka->meta;
    while (blk_meta != NULL)
    {
        struct blk_meta *tmp = blk_meta;
        blk_meta = blk_meta->next;
        blka_free(tmp);
    }
    free(blka);
}

struct blk_meta *blka_alloc(struct blk_allocator *blka, size_t size)
{
    size_t page_size = sysconf(_SC_PAGESIZE);
    if (size < page_size)
        size = page_size - sizeof(struct blk_meta);

    struct blk_meta *blk = (struct blk_meta *)mmap(
        NULL, size * sizeof(struct blk_meta), PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (blk == MAP_FAILED)
        return NULL;

    if (blka->meta == NULL)
        blk->next = NULL;
    else
        blk->next = blka->meta;
    blka->meta = blk;

    blk->size = size;

    return blk;
}
void blka_free(struct blk_meta *blk)
{
    munmap(blk, blk->size);
}
void blka_pop(struct blk_allocator *blka)
{
    if (blka != NULL && blka->meta != NULL)
    {
        struct blk_meta *tmp = blka->meta;
        blka->meta = blka->meta->next;
        blka_free(tmp);
    }
}