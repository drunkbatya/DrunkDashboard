#pragma once

#include <gui/scene_manager_i.h>
#include <gui/gui.h>
#include <halk/halk.h>
#include "scenes/drunk_dashboard_scene.h"
#include "views/drunk_dashboard_view_pressure.h"
#include "views/drunk_dashboard_view_debug.h"

typedef struct {
    Gui* gui;
    SceneManager* scene_manager;
    DrunkDashboardViewPressure* view_pressure;
    DrunkDashboardViewDebug* view_debug;
} DrunkDashboard;

DrunkDashboard* drunk_dashboard_alloc();
void drunk_dashboard_run(DrunkDashboard* drunk_dashboard);
void drunk_dashboard_free(DrunkDashboard* drunk_dashboard);
