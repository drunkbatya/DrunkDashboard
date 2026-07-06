#pragma once

#include <gui/view.h>
#include <input/input.h>

typedef struct DashboardViewDashboard DashboardViewDashboard;
typedef void (*DashboardViewDashboardCallback)(void* context, InputKey key);

DashboardViewDashboard* dashboard_view_dashboard_alloc(void);
View* dashboard_view_dashboard_get_view(DashboardViewDashboard* dashboard_view_dashboard);
void dashboard_view_dashboard_set_callback(
    DashboardViewDashboard* dashboard_view_dashboard,
    DashboardViewDashboardCallback callback,
    void* context);

void dashboard_view_dashboard_free(DashboardViewDashboard* dashboard_view_dashboard);
void dashboard_view_dashboard_update(DashboardViewDashboard* dashboard_view_dashboard);
