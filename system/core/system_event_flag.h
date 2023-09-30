#pragma once

#include "base.h"

typedef void SystemEventFlag;

SystemEventFlag* system_event_flag_alloc();
void system_event_flag_free(SystemEventFlag* instance);
uint32_t system_event_flag_set(SystemEventFlag* instance, uint32_t flags);
uint32_t system_event_flag_clear(SystemEventFlag* instance, uint32_t flags);
uint32_t system_event_flag_get(SystemEventFlag* instance);
uint32_t system_event_flag_wait(
    SystemEventFlag* instance,
    uint32_t flags,
    uint32_t options,
    uint32_t timeout);
