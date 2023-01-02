#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h>

#include "utils.h"

extern void *base;

__attribute__((visibility("default"))) void *malloc(size_t size);

__attribute__((visibility("default"))) void free(void *ptr);

__attribute__((visibility("default"))) void *realloc(void *ptr, size_t size);

__attribute__((visibility("default"))) void *calloc(size_t nmemb, size_t size);

#endif /* !ALLOCATOR_H */