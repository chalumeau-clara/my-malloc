#include "page_begin.h"
#include <stdio.h>

void *page_begin(void *ptr, size_t page_size)
{
    char *tmp = ptr;
    char *page = 0;
    while (page + page_size < tmp)
        page += page_size;
    return page;
}