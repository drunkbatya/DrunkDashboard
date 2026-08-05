#include "../dashboard_app.h"
#include "../decoders/ecu_decoder.h"
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
        const EcuDecoder* decoder = app->ecu_decoder;
        dashboard_can_worker_get_frames(
            app->can_worker,
            decoder->frame_ids,
            app->ecu_can_frames->frames,
            app->ecu_can_frames->found,
            decoder->frame_count);
        decoder->decode(app->ecu_can_frames, app->ecu_state);
        dashboard_view_dashboard_update(app->view_dashboard, app->ecu_state);
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
