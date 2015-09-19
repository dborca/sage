/**
 * Allocate aligned memory.
 * The aligned block MUST be freed with functions provided here.
 */


#include <assert.h>
#include <stdlib.h>

#include "alloc.h"


void *
malloc_a (size_t size, int p2align)
{
    void *ptr, **block;
    size_t align = (1 << p2align) - 1;
    size_t len = size + sizeof(void *) + align;
    ptr = malloc(len);
    if (ptr == NULL) {
	return NULL;
    }
    block = (void **)(((long)ptr + sizeof(void *) + align) & ~align);
#if 1
    assert((long)block - (long)ptr >= (long)sizeof(void *));
    assert(((long)block + size) <= ((long)ptr + len));
    assert(((long)block & align) == 0);
#endif
    block[-1] = ptr;
    return block;
}


void *
calloc_a (size_t size, int p2align)
{
    void *ptr, **block;
    size_t align = (1 << p2align) - 1;
    size_t len = size + sizeof(void *) + align;
    ptr = calloc(1, len);
    if (ptr == NULL) {
	return NULL;
    }
    block = (void **)(((long)ptr + sizeof(void *) + align) & ~align);
#if 1
    assert((long)block - (long)ptr >= (long)sizeof(void *));
    assert(((long)block + size) <= ((long)ptr + len));
    assert(((long)block & align) == 0);
#endif
    block[-1] = ptr;
    return block;
}


void
free_a (void *ptr)
{
    if (ptr != NULL) {
	free(((void **)ptr)[-1]);
    }
}
