#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdint.h>
#include <sys/mman.h>

// Multiple of 4
#define align4(x) (((((x)-1) >> 2) << 2) + 4)
#define BLOCK_SIZE 20

extern void *base;

typedef struct block *p_block;

struct block
{
    size_t size;
    struct block *next;
    struct block *prev;
    int free;
    void *ptr;
    char data[1]; // allocate block
};

p_block find_block(p_block *last, size_t size);
p_block create_block(p_block last, size_t size);
void split_block(p_block b, size_t size);
p_block fusion_block(p_block b);
p_block get_block(void *p);
int valid_addr(void *p, p_block b);
void copy_block(p_block src, p_block dst);

#endif /* !ALLOCATOR_H */