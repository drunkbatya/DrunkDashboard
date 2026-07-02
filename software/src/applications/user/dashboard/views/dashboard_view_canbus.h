#pragma once

#include <stdint.h>

#include <gui/view.h>
#include "../workers/dashboard_can_worker.h"

typedef struct DashboardViewCanbus DashboardViewCanbus;

DashboardViewCanbus* dashboard_view_canbus_view_alloc(void);
View* dashboard_view_canbus_get_view(DashboardViewCanbus* dashboard_view_canbus);

void dashboard_view_canbus_view_free(DashboardViewCanbus* dashboard_view_canbus);
void dashboard_view_canbus_update(
    DashboardViewCanbus* dashboard_view_canbus,
    const DashboardCanFrame* frame);
