#include "../dashboard_app.h"
#include "../views/dashboard_view_main.h"
#include <furi.h>

static void dashboard_scene_canbus_submenu_callback(void* context, uint32_t index);

static void dashboard_scene_canbus_rebuild_submenu(DashboardApp* app) {
    app->can_frames_count = dashboard_can_worker_copy_snapshot(
        app->can_worker, app->can_frames, DASHBOARD_CAN_MAX_IDS);

    uint32_t selected_id = submenu_get_selected_item(app->canbus_submenu);

    submenu_reset(app->canbus_submenu);
    submenu_set_header(app->canbus_submenu, "CAN messages");

    if(app->can_frames_count == 0) {
        submenu_add_item(app->canbus_submenu, "No messages", 0, NULL, app);
        return;
    }

    for(size_t i = 0; i < app->can_frames_count; i++) {
        char label[32];
        snprintf(label, sizeof(label), "0x%03lX", (unsigned long)app->can_frames[i].id);
        submenu_add_item(
            app->canbus_submenu,
            label,
            app->can_frames[i].id,
            dashboard_scene_canbus_submenu_callback,
            app);
    }

    submenu_set_selected_item(app->canbus_submenu, selected_id);
}

void dashboard_scene_canbus_data_updated_callback(void* context) {
    furi_assert(context);
    DashboardApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, DashboardAppCustomEventDataUpdated);
}

static void dashboard_scene_canbus_submenu_callback(void* context, uint32_t index) {
    furi_assert(context);
    DashboardApp* app = context;
    app->selected_can_id = index;
    view_dispatcher_send_custom_event(app->view_dispatcher, DashboardAppCustomEventCanIdSelected);
}

bool dashboard_scene_canbus_on_event(void* context, SceneManagerEvent event) {
    DashboardApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == DashboardAppCustomEventDataUpdated) {
            //dashboard_scene_canbus_rebuild_submenu(app);
            //consumed = true;
        } else if(event.event == DashboardAppCustomEventCanIdSelected) {
            scene_manager_next_scene(app->scene_manager, DashboardSceneCanbusDetail);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeTick) {
        dashboard_scene_canbus_rebuild_submenu(app);
        consumed = true;
    }
    return consumed;
}

void dashboard_scene_canbus_on_enter(void* context) {
    DashboardApp* app = context;

    //dashboard_can_worker_set_data_updated_callback(
    //    app->can_worker, dashboard_scene_canbus_data_updated_callback, app);

    submenu_reset(app->canbus_submenu);
    dashboard_scene_canbus_rebuild_submenu(app);

    view_dispatcher_switch_to_view(app->view_dispatcher, DashboardAppViewCanbusSubmenu);
}

void dashboard_scene_canbus_on_exit(void* context) {
    UNUSED(context);
}
