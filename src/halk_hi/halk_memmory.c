#include <stddef.h>
#include <stdlib.h>

void* __wrap_malloc(size_t size) {
    return calloc(size, 1);
}
