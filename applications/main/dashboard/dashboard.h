#pragma once

#include <gui/scene_manager_i.h>
#include <gui/gui.h>
#include <system_core_defs.h>
#include "scenes/dashboard_scene.h"
#include "views/dashboard_view_pressure.h"
#include "views/dashboard_view_debug.h"

typedef struct {
    Gui* gui;
    SceneManager* scene_manager;
    DashboardViewPressure* view_pressure;
    DashboardViewDebug* view_debug;
} Dashboard;

void init_thread();
