#include "utils.h"

p_block find_block(p_block *last, size_t size)
{
    p_block b = base;
    while (b && !(b->free && b->size >= size))
    {
        *last = b;
        b = b->next;
    }
    return b;
}

p_block create_block(p_block last, size_t size)
{
    p_block b;
    b = mmap(NULL, size + BLOCK_SIZE, PROT_READ | PROT_WRITE,
             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (b == MAP_FAILED)
        return NULL;
    b->size = size;
    b->next = NULL;
    b->prev = last;
    b->free = 0;
    b->ptr = b->data;

    if (last)
        last->next = b;

    return b;
}

void split_block(p_block b, size_t size)
{
    p_block new;
    new = (p_block)(b->data + size);
    new->size = b->size - size - BLOCK_SIZE;
    new->next = b->next;
    new->prev = b;
    new->free = 1;
    new->ptr = new->data;
    b->size = size;
    b->next = new;
    if (new->next)
        new->next->prev = new;
}

p_block fusion_block(p_block b)
{
    if (b->next && b->next->free)
    {
        b->size += BLOCK_SIZE + b->next->size;
        b->next = b->next->next;
        if (b->next)
            b->next->prev = b;
    }
    return b;
}

p_block get_block(void *p)
{
    char *tmp;
    tmp = p;
    return (p = tmp -= BLOCK_SIZE);
}

int valid_addr(void *p, p_block b)
{
    if (base)
    {
        if (p > base && p <= b->ptr)
            return p == get_block(p)->ptr;
    }
    return 0;
}

struct list *create_metadata_page()
{
    struct list *list =
        (struct list *)mmap(NULL, INT64_MAX, PROT_READ | PROT_WRITE,
                            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (list == MAP_FAILED)
        return NULL;
    return list;
}

void copy_block(p_block src, p_block dst)
{
    int *sdata = src->ptr;
    int *ddata = dst->ptr;
    for (size_t i = 0; i * 4 < src->size && i * 4 < dst->size; i++)
        ddata[i] = sdata[i];
}
