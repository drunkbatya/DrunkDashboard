#include <system_core_defs.h>
#include "system_memmory.h"
#include "system_memmory_heap.h"

void* malloc(size_t size) {
    void* ptr = pvPortMalloc(size);
    memset(ptr, 0, size);
    return ptr;
}

void free(void* ptr) {
    vPortFree(ptr);
}

void* realloc(void* ptr, size_t size) {
    if(size == 0) {
        vPortFree(ptr);
        return NULL;
    }

    void* p = pvPortMalloc(size);
    if(ptr != NULL) {
        memcpy(p, ptr, size);
        vPortFree(ptr);
    }

    return p;
}

void* calloc(size_t count, size_t size) {
    return pvPortMalloc(count * size);
}

void* __wrap__malloc_r(struct _reent* r, size_t size) {
    UNUSED(r);
    return pvPortMalloc(size);
}

void __wrap__free_r(struct _reent* r, void* ptr) {
    UNUSED(r);
    vPortFree(ptr);
}

void* __wrap__calloc_r(struct _reent* r, size_t count, size_t size) {
    UNUSED(r);
    return calloc(count, size);
}

void* __wrap__realloc_r(struct _reent* r, void* ptr, size_t size) {
    UNUSED(r);
    return realloc(ptr, size);
}

size_t system_memmory_get_free_heap() {
    return (xPortGetFreeHeapSize());
}

size_t system_memmory_get_total_heap() {
    return xPortGetTotalHeapSize();
}
