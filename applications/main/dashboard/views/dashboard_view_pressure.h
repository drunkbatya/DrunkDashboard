#pragma once

#include <gui/view.h>

typedef struct DashboardViewPressure DashboardViewPressure;

DashboardViewPressure* dashboard_view_pressure_alloc();
void dashboard_view_pressure_free(DashboardViewPressure* app);
View* dashboard_view_pressure_get_view(DashboardViewPressure* app);
void dashboard_view_pressure_set_pressure(DashboardViewPressure* app, uint16_t pressure);
