#ifndef ALLOC_H_included
#define ALLOC_H_included

void *malloc_a (size_t size, int p2align);
void *calloc_a (size_t size, int p2align);
void free_a (void *ptr);

#endif
