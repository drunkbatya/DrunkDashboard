#include <dashboard/dashboard.h>

void dashboard_scene_main_on_enter(void* context) {
    Dashboard* app = context;
    gui_set_view_on_display(app->gui, dashboard_view_debug_get_view(app->view_debug), 0);
    gui_set_view_on_display(app->gui, dashboard_view_debug_get_view(app->view_debug), 1);
    gui_set_view_on_display(app->gui, dashboard_view_debug_get_view(app->view_debug), 2);
    gui_set_view_on_display(
        app->gui, dashboard_view_pressure_get_view(app->view_pressure), 3);
    gui_set_view_on_display(
        app->gui, dashboard_view_pressure_get_view(app->view_pressure), 4);
    gui_set_view_on_display(
        app->gui, dashboard_view_pressure_get_view(app->view_pressure), 5);
    gui_set_view_on_display(app->gui, dashboard_view_debug_get_view(app->view_debug), 6);
    gui_set_view_on_display(app->gui, dashboard_view_debug_get_view(app->view_debug), 7);
    gui_set_view_on_display(app->gui, dashboard_view_debug_get_view(app->view_debug), 8);
}

bool dashboard_scene_main_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return true;
}

void dashboard_scene_main_on_exit(void* context) {
    Dashboard* app = context;
    gui_remove_views_from_all_displays(app->gui);
}
