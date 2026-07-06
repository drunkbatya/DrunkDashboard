#include "../dashboard_app.h"
#include "../views/dashboard_view_dashboard.h"
#include <furi.h>

static void dashboard_scene_dashboard_view_callback(void* context, InputKey key) {
    DashboardApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, key);
}

bool dashboard_scene_dashboard_on_event(void* context, SceneManagerEvent event) {
    DashboardApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == InputKeyOk) {
            scene_manager_next_scene(app->scene_manager, DashboardSceneMenu);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeTick) {
        dashboard_view_dashboard_update(app->view_dashboard);
        consumed = true;
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = true;
    }
    return consumed;
}

void dashboard_scene_dashboard_on_enter(void* context) {
    DashboardApp* app = context;
    dashboard_view_dashboard_set_callback(
        app->view_dashboard, dashboard_scene_dashboard_view_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, DashboardAppViewDashboard);
}

void dashboard_scene_dashboard_on_exit(void* context) {
    DashboardApp* app = context;
    dashboard_view_dashboard_set_callback(app->view_dashboard, NULL, NULL);
}
