#pragma once

#include <gui/view.h>

typedef struct DrunkDashboardViewDebug DrunkDashboardViewDebug;

DrunkDashboardViewDebug* drunk_dashboard_view_debug_alloc();
void drunk_dashboard_view_debug_free(DrunkDashboardViewDebug* app);
View* drunk_dashboard_view_debug_get_view(DrunkDashboardViewDebug* app);
