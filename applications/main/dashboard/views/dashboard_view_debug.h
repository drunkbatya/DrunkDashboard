#pragma once

#include <gui/view.h>

typedef struct DashboardViewDebug DashboardViewDebug;

DashboardViewDebug* dashboard_view_debug_alloc();
void dashboard_view_debug_free(DashboardViewDebug* app);
View* dashboard_view_debug_get_view(DashboardViewDebug* app);
