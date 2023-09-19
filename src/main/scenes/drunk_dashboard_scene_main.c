#include <main/drunk_dashboard.h>

void drunk_dashboard_scene_main_on_enter(void* context) {
    DrunkDashboard* app = context;
    gui_set_view_on_display(
        app->gui, drunk_dashboard_view_pressure_get_view(app->view_pressure), 1);
    gui_set_view_on_display(
        app->gui, drunk_dashboard_view_pressure_get_view(app->view_pressure), 3);
}

bool drunk_dashboard_scene_main_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return true;
}

void drunk_dashboard_scene_main_on_exit(void* context) {
    DrunkDashboard* app = context;
    gui_remove_views_from_all_displays(app->gui);
}
