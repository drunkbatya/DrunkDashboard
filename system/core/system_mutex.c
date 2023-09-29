#include <FreeRTOS.h>
#include "system_mutex.h"
#include <semphr.h>

SystemMutex* system_mutex_alloc(SystemMutexType type) {
    SemaphoreHandle_t hMutex = NULL;
    if(type == SystemMutexTypeNormal) {
        hMutex = xSemaphoreCreateMutex();
    } else if(type == SystemMutexTypeRecursive) {
        hMutex = xSemaphoreCreateRecursiveMutex();
    }
    if(type == SystemMutexTypeRecursive) {
        /* Set LSB as 'recursive mutex flag' */
        hMutex = (SemaphoreHandle_t)((uint32_t)hMutex | 1U);
    }
    return ((SystemMutex*)hMutex);
}

void system_mutex_free(SystemMutex* mutex) {
    vSemaphoreDelete((SemaphoreHandle_t)((uint32_t)mutex & ~1U));
}

bool system_mutex_release(SystemMutex* mutex) {
    SemaphoreHandle_t hMutex;
    uint32_t rmtx;

    hMutex = (SemaphoreHandle_t)((uint32_t)mutex & ~1U);

    /* Extract recursive mutex flag */
    rmtx = (uint32_t)mutex & 1U;

    if(rmtx != 0U) {
        return (xSemaphoreGiveRecursive(hMutex) == pdPASS);
    } else {
        return (xSemaphoreGive(hMutex) == pdPASS);
    }
}

bool system_mutex_acquire(SystemMutex* instance, uint32_t timeout) {
    SemaphoreHandle_t hMutex;
    uint32_t rmtx;

    hMutex = (SemaphoreHandle_t)((uint32_t)instance & ~1U);

    /* Extract recursive mutex flag */
    rmtx = (uint32_t)instance & 1U;

    if(rmtx != 0U) {
        return (xSemaphoreTakeRecursive(hMutex, timeout) != pdPASS);
    } else {
        return (xSemaphoreTake(hMutex, timeout) != pdPASS);
    }
}
