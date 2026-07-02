#include "../dashboard_app.h"

#include <furi.h>

static void dashboard_scene_canbus_detail_update(DashboardApp* app) {
    DashboardCanFrame frame;
    if(dashboard_can_worker_get_frame(app->can_worker, app->selected_can_id, &frame)) {
        dashboard_view_canbus_update(app->view_canbus, &frame);
    } else {
        dashboard_view_canbus_update(app->view_canbus, NULL);
    }
}

void dashboard_scene_canbus_detail_on_enter(void* context) {
    DashboardApp* app = context;
    dashboard_scene_canbus_detail_update(app);
    view_dispatcher_switch_to_view(app->view_dispatcher, DashboardAppViewCanbus);
}

bool dashboard_scene_canbus_detail_on_event(void* context, SceneManagerEvent event) {
    DashboardApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeTick) {
        dashboard_scene_canbus_detail_update(app);
        consumed = true;
    }

    return consumed;
}

void dashboard_scene_canbus_detail_on_exit(void* context) {
    UNUSED(context);
}
