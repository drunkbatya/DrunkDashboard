#pragma once

#include <gui/view.h>

typedef struct DrunkDashboardViewPressure DrunkDashboardViewPressure;

DrunkDashboardViewPressure* drunk_dashboard_view_pressure_alloc();
void drunk_dashboard_view_pressure_free(DrunkDashboardViewPressure* app);
View* drunk_dashboard_view_pressure_get_view(DrunkDashboardViewPressure* app);
void drunk_dashboard_view_pressure_set_pressure(DrunkDashboardViewPressure* app, uint16_t pressure);
