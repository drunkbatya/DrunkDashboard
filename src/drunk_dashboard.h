#pragma once

#include <scene_manager_i.h>
#include <halk/halk.h>
#include <scenes/drunk_dashboard_scene.h>

typedef struct {
    SceneManager* scene_manager;
} DrunkDashboard;

DrunkDashboard* drunk_dashboard_alloc();
void drunk_dashboard_run(DrunkDashboard* drunk_dashboard);
void drunk_dashboard_free(DrunkDashboard* drunk_dashboard);
