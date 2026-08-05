#pragma once

#include "../decoders/ecu_decoder.h"

#include <gui/view.h>
#include <input/input.h>

typedef struct DashboardViewDashboard DashboardViewDashboard;
typedef void (*DashboardViewDashboardCallback)(void* context, InputKey key);

DashboardViewDashboard* dashboard_view_dashboard_alloc(void);
void dashboard_view_dashboard_set_callback(
    DashboardViewDashboard* view_dashboard,
    DashboardViewDashboardCallback callback,
    void* context);
void dashboard_view_dashboard_free(DashboardViewDashboard* view_dashboard);
View* dashboard_view_dashboard_get_view(DashboardViewDashboard* view_dashboard);
void dashboard_view_dashboard_update(DashboardViewDashboard* view_dashboard, const EcuState* state);
