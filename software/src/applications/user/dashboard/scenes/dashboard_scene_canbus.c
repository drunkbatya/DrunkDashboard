#include "../dashboard_app.h"
#include <furi.h>

static void dashboard_scene_canbus_submenu_callback(void* context, uint32_t index);

static void dashboard_scene_canbus_rebuild_submenu(DashboardApp* app, uint32_t selected_id) {
    app->can_monitor_frames_count = dashboard_can_worker_copy_snapshot(
        app->can_worker, app->can_monitor_frames, DASHBOARD_CAN_MAX_IDS);

    submenu_reset(app->canbus_submenu);
    submenu_set_header(app->canbus_submenu, "CAN messages");

    if(app->can_monitor_frames_count == 0) {
        submenu_add_item(app->canbus_submenu, "No messages", 0, NULL, app);
        submenu_set_selected_item(app->canbus_submenu, 0);
        return;
    }

    for(size_t i = 0; i < app->can_monitor_frames_count; i++) {
        furi_string_printf(app->text, "0x%03lX", (unsigned long)app->can_monitor_frames[i].id);
        submenu_add_item(
            app->canbus_submenu,
            furi_string_get_cstr(app->text),
            app->can_monitor_frames[i].id,
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
            scene_manager_set_scene_state(
                app->scene_manager, DashboardSceneCanbus, app->selected_can_id);
            scene_manager_next_scene(app->scene_manager, DashboardSceneCanbusDetail);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeTick) {
        dashboard_scene_canbus_rebuild_submenu(
            app, submenu_get_selected_item(app->canbus_submenu));
        consumed = true;
    }
    return consumed;
}

void dashboard_scene_canbus_on_enter(void* context) {
    DashboardApp* app = context;

    //dashboard_can_worker_set_data_updated_callback(
    //    app->can_worker, dashboard_scene_canbus_data_updated_callback, app);

    submenu_reset(app->canbus_submenu);
    dashboard_scene_canbus_rebuild_submenu(
        app, scene_manager_get_scene_state(app->scene_manager, DashboardSceneCanbus));

    view_dispatcher_switch_to_view(app->view_dispatcher, DashboardAppViewCanbusSubmenu);
}

void dashboard_scene_canbus_on_exit(void* context) {
    DashboardApp* app = context;
    scene_manager_set_scene_state(
        app->scene_manager, DashboardSceneCanbus, submenu_get_selected_item(app->canbus_submenu));
}
