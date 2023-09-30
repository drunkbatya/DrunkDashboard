#include "system_event_flag.h"
#include "system_core_defs.h"
#include "system_check.h"

#include <FreeRTOS.h>
#include <event_groups.h>

#define SYSTEM_EVENT_FLAG_MAX_BITS_EVENT_GROUPS 24U
#define SYSTEM_EVENT_FLAG_INVALID_BITS (~((1UL << SYSTEM_EVENT_FLAG_MAX_BITS_EVENT_GROUPS) - 1U))

SystemEventFlag* system_event_flag_alloc() {
    system_assert(!SYSTEM_IS_IRQ_MODE());

    EventGroupHandle_t handle = xEventGroupCreate();
    system_check(handle);

    return ((SystemEventFlag*)handle);
}

void system_event_flag_free(SystemEventFlag* instance) {
    system_assert(!SYSTEM_IS_IRQ_MODE());
    vEventGroupDelete((EventGroupHandle_t)instance);
}

uint32_t system_event_flag_set(SystemEventFlag* instance, uint32_t flags) {
    system_assert(instance);
    system_assert((flags & SYSTEM_EVENT_FLAG_INVALID_BITS) == 0U);

    EventGroupHandle_t hEventGroup = (EventGroupHandle_t)instance;
    uint32_t rflags;
    BaseType_t yield;

    if(SYSTEM_IS_IRQ_MODE()) {
        yield = pdFALSE;
        if(xEventGroupSetBitsFromISR(hEventGroup, (EventBits_t)flags, &yield) == pdFAIL) {
            rflags = (uint32_t)SystemFlagErrorResource;
        } else {
            rflags = flags;
            portYIELD_FROM_ISR(yield);
        }
    } else {
        rflags = xEventGroupSetBits(hEventGroup, (EventBits_t)flags);
    }

    /* Return event flags after setting */
    return (rflags);
}

uint32_t system_event_flag_clear(SystemEventFlag* instance, uint32_t flags) {
    system_assert(instance);
    system_assert((flags & SYSTEM_EVENT_FLAG_INVALID_BITS) == 0U);

    EventGroupHandle_t hEventGroup = (EventGroupHandle_t)instance;
    uint32_t rflags;

    if(SYSTEM_IS_IRQ_MODE()) {
        rflags = xEventGroupGetBitsFromISR(hEventGroup);

        if(xEventGroupClearBitsFromISR(hEventGroup, (EventBits_t)flags) == pdFAIL) {
            rflags = (uint32_t)SystemStatusErrorResource;
        } else {
            /* xEventGroupClearBitsFromISR only registers clear operation in the timer command queue. */
            /* Yield is required here otherwise clear operation might not execute in the right order. */
            /* See https://github.com/FreeRTOS/FreeRTOS-Kernel/issues/93 for more info.               */
            portYIELD_FROM_ISR(pdTRUE);
        }
    } else {
        rflags = xEventGroupClearBits(hEventGroup, (EventBits_t)flags);
    }

    /* Return event flags before clearing */
    return (rflags);
}

uint32_t system_event_flag_get(SystemEventFlag* instance) {
    system_assert(instance);

    EventGroupHandle_t hEventGroup = (EventGroupHandle_t)instance;
    uint32_t rflags;

    if(SYSTEM_IS_IRQ_MODE()) {
        rflags = xEventGroupGetBitsFromISR(hEventGroup);
    } else {
        rflags = xEventGroupGetBits(hEventGroup);
    }

    /* Return current event flags */
    return (rflags);
}

uint32_t system_event_flag_wait(
    SystemEventFlag* instance,
    uint32_t flags,
    uint32_t options,
    uint32_t timeout) {
    system_assert(!SYSTEM_IS_IRQ_MODE());
    system_assert(instance);
    system_assert((flags & SYSTEM_EVENT_FLAG_INVALID_BITS) == 0U);

    EventGroupHandle_t hEventGroup = (EventGroupHandle_t)instance;
    BaseType_t wait_all;
    BaseType_t exit_clr;
    uint32_t rflags;

    if(options & SystemFlagWaitAll) {
        wait_all = pdTRUE;
    } else {
        wait_all = pdFAIL;
    }

    if(options & SystemFlagNoClear) {
        exit_clr = pdFAIL;
    } else {
        exit_clr = pdTRUE;
    }

    rflags = xEventGroupWaitBits(
        hEventGroup, (EventBits_t)flags, exit_clr, wait_all, (TickType_t)timeout);

    if(options & SystemFlagWaitAll) {
        if((flags & rflags) != flags) {
            if(timeout > 0U) {
                rflags = (uint32_t)SystemStatusErrorTimeout;
            } else {
                rflags = (uint32_t)SystemStatusErrorResource;
            }
        }
    } else {
        if((flags & rflags) == 0U) {
            if(timeout > 0U) {
                rflags = (uint32_t)SystemStatusErrorTimeout;
            } else {
                rflags = (uint32_t)SystemStatusErrorResource;
            }
        }
    }

    /* Return event flags before clearing */
    return (rflags);
}
