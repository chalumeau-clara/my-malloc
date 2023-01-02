#include "beware_overflow.h"

void *beware_overflow(void *ptr, size_t nmemb, size_t size)
{
    size_t mult;
    char *pptr = ptr;
    if (__builtin_mul_overflow(nmemb, size, &mult))
        return NULL;
    return pptr + mult;
}