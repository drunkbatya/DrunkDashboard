#include <FreeRTOS.h>
#include <semphr.h>
#include "system_mutex.h"
#include "system_check.h"
#include "system_core_defs.h"

SystemMutex* system_mutex_alloc(SystemMutexType type) {
    system_assert(!SYSTEM_IS_IRQ_MODE());
    SemaphoreHandle_t hMutex = NULL;
    if(type == SystemMutexTypeNormal) {
        hMutex = xSemaphoreCreateMutex();
    } else if(type == SystemMutexTypeRecursive) {
        hMutex = xSemaphoreCreateRecursiveMutex();
    } else {
        system_crash("Programming error");
    }
    system_check(hMutex != NULL);
    if(type == SystemMutexTypeRecursive) {
        /* Set LSB as 'recursive mutex flag' */
        hMutex = (SemaphoreHandle_t)((uint32_t)hMutex | 1U);
    }
    return ((SystemMutex*)hMutex);
}

void system_mutex_free(SystemMutex* mutex) {
    system_assert(!SYSTEM_IS_IRQ_MODE());
    system_assert(mutex);
    vSemaphoreDelete((SemaphoreHandle_t)((uint32_t)mutex & ~1U));
}

SystemStatus system_mutex_release(SystemMutex* instance) {
    SemaphoreHandle_t hMutex;
    SystemStatus stat;
    uint32_t rmtx;

    hMutex = (SemaphoreHandle_t)((uint32_t)instance & ~1U);

    /* Extract recursive mutex flag */
    rmtx = (uint32_t)instance & 1U;

    stat = SystemStatusOk;

    if(SYSTEM_IS_IRQ_MODE()) {
        stat = SystemStatusErrorISR;
    } else if(hMutex == NULL) {
        stat = SystemStatusErrorParameter;
    } else {
        if(rmtx != 0U) {
            if(xSemaphoreGiveRecursive(hMutex) != pdPASS) {
                stat = SystemStatusErrorResource;
            }
        } else {
            if(xSemaphoreGive(hMutex) != pdPASS) {
                stat = SystemStatusErrorResource;
            }
        }
    }

    /* Return execution status */
    return (stat);
}

SystemStatus system_mutex_acquire(SystemMutex* instance, uint32_t timeout) {
    SemaphoreHandle_t hMutex;
    SystemStatus stat;
    uint32_t rmtx;

    hMutex = (SemaphoreHandle_t)((uint32_t)instance & ~1U);

    /* Extract recursive mutex flag */
    rmtx = (uint32_t)instance & 1U;

    stat = SystemStatusOk;

    if(SYSTEM_IS_IRQ_MODE()) {
        stat = SystemStatusErrorISR;
    } else if(hMutex == NULL) {
        stat = SystemStatusErrorParameter;
    } else {
        if(rmtx != 0U) {
            if(xSemaphoreTakeRecursive(hMutex, timeout) != pdPASS) {
                if(timeout != 0U) {
                    stat = SystemStatusErrorTimeout;
                } else {
                    stat = SystemStatusErrorResource;
                }
            }
        } else {
            if(xSemaphoreTake(hMutex, timeout) != pdPASS) {
                if(timeout != 0U) {
                    stat = SystemStatusErrorTimeout;
                } else {
                    stat = SystemStatusErrorResource;
                }
            }
        }
    }
    /* Return execution status */
    return (stat);
}
