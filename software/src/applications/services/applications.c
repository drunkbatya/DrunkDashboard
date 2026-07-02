#include "applications.h"

int32_t input_srv(void* p);
int32_t gui_srv(void* p);
int32_t dashboard_app(void* p);

const DrunkDashboardInternalApplication DRUNK_DASHBOARD_SERVICES[] = {
    {
        .app = input_srv,
        .name = "InputSrv",
        .appid = "input",
        .stack_size = 1024,
        .icon = NULL,
        .flags = DrunkDashboardInternalApplicationFlagDefault,
    },
    {
        .app = gui_srv,
        .name = "GuiSrv",
        .appid = "gui",
        .stack_size = 2048,
        .icon = NULL,
        .flags = DrunkDashboardInternalApplicationFlagDefault,
    },
    {
        .app = dashboard_app,
        .name = "Dashboard",
        .appid = "dashboard",
        .stack_size = 4096,
        .icon = NULL,
        .flags = DrunkDashboardInternalApplicationFlagDefault,
    },
};

const size_t DRUNK_DASHBOARD_SERVICES_COUNT = COUNT_OF(DRUNK_DASHBOARD_SERVICES);
