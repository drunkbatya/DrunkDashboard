#pragma once

#include <furi.h>
#include <gui/icon.h>

typedef enum {
    DrunkDashboardInternalApplicationFlagDefault = 0,
    DrunkDashboardInternalApplicationFlagInsomniaSafe = (1 << 0),
} DrunkDashboardInternalApplicationFlag;

typedef struct {
    const FuriThreadCallback app;
    const char* name;
    const char* appid;
    const size_t stack_size;
    const Icon* icon;
    const DrunkDashboardInternalApplicationFlag flags;
} DrunkDashboardInternalApplication;

extern const DrunkDashboardInternalApplication DRUNK_DASHBOARD_SERVICES[];
extern const size_t DRUNK_DASHBOARD_SERVICES_COUNT;
