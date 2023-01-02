#include "malloc.h"

#include <stddef.h>
#include <string.h>

#include "utils.h"
__attribute__((visibility("default"))) void *base = NULL;

__attribute__((visibility("default"))) void *malloc(size_t size)
{
    p_block b, last;
    size = align4(size);

    // if base exist
    if (base)
    {
        // find the block
        last = base;
        b = find_block(&last, size);

        // if fitting block
        if (b)
        {
            // if we can split
            if ((b->size - size) >= (BLOCK_SIZE + 4))
                split_block(b, size);
            b->free = 0;
        }
        // Create a new block
        else
        {
            b = create_block(last, size);
            if (!b)
                return NULL;
        }
    }
    else
    {
        // if base do not exist
        b = create_block(NULL, size);
        if (!b)
            return NULL;
        base = b;
    }
    return b->data;
}

void *mallocrea(size_t size)
{
    p_block b, last;
    size = align4(size);

    // if base exist
    if (base)
    {
        // find the block
        last = base;
        b = find_block(&last, size);

        // if fitting block
        if (b)
        {
            // if we can split
            if ((b->size - size) >= (BLOCK_SIZE + 4))
                split_block(b, size);
            b->free = 0;
        }
        // Create a new block
        else
        {
            b = create_block(last, size);
            if (!b)
                return NULL;
        }
    }
    else
    {
        // if base do not exist
        b = create_block(NULL, size);
        if (!b)
            return NULL;
        base = b;
    }
    return b->data;
}

__attribute__((visibility("default"))) void free(void *ptr)
{
    p_block b;

    // verify valid address
    if (valid_addr(ptr, base))
    {
        b = get_block(ptr);
        b->free = 1;

        // if fusion with prev possible
        if (b->prev && b->prev->free)
            b = fusion_block(b->prev);

        // if fusion with next possible
        if (b->next && b->next->free)
            b = fusion_block(b);
        else
        {
            // free end of the list
            if (b->prev)
                b->prev->next = NULL;
            // free block
            else
                base = NULL;
            munmap(b, b->size);
        }
    }
}

__attribute__((visibility("default"))) void *realloc(void *ptr, size_t size)
{
    if (!ptr)
        return mallocrea(size);

    if (valid_addr(ptr, base))
    {
        size = align4(size);
        p_block b = get_block(ptr);
        if (b->size >= size)
        {
            if (b->size - size >= (BLOCK_SIZE + 4))
                split_block(b, size);
        }
        else
        {
            // try fusion with next
            if (b->next && b->next->free
                && (b->size + BLOCK_SIZE + b->next->size) >= size)
            {
                fusion_block(b);
                if (b->size - size >= (BLOCK_SIZE + 4))
                    split_block(b, size);
            }
            else
            {
                // new block
                void *newp = mallocrea(size);
                if (!newp)
                    return NULL;
                p_block new = get_block(newp);
                copy_block(b, new);
                free(ptr);
                return newp;
                // void *newp = mremap(b->data, b->size, size, 0);
            }
        }
        return ptr;
    }

    return NULL;
}

__attribute__((visibility("default"))) void *calloc(size_t nmemb, size_t size)
{
    size_t *new;
    size_t mult;

    if (!__builtin_mul_overflow(nmemb, size, &mult))
    {
        new = mallocrea(mult);

        if (new)
        {
            memset(new, 0, mult);
            /*size_t align = align4(mult) << 2;
            fprintf(stderr, "%ld\n", align);
            fprintf(stderr, "%ld\n", mult);
            for (size_t i = 0; i < align; i++) {
                //fprintf(stderr, "%ld\n", i);
                new[i] = 0;
            }*/
        }

        return new;
    }
    return NULL;
}
