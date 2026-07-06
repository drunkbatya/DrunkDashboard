#include "../dashboard_app.h"

#include <furi.h>

typedef enum {
    DashboardSettingsItemLcd,
} DashboardSettingsItem;

static void dashboard_scene_settings_submenu_callback(void* context, uint32_t index) {
    DashboardApp* app = context;
    scene_manager_set_scene_state(app->scene_manager, DashboardSceneSettings, index);
    view_dispatcher_send_custom_event(
        app->view_dispatcher, DashboardAppCustomEventSettingsSelected);
}

bool dashboard_scene_settings_on_event(void* context, SceneManagerEvent event) {
    DashboardApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == DashboardAppCustomEventSettingsSelected) {
            uint32_t selected =
                scene_manager_get_scene_state(app->scene_manager, DashboardSceneSettings);
            if(selected == DashboardSettingsItemLcd) {
                scene_manager_next_scene(app->scene_manager, DashboardSceneSettingsLcd);
            }
            consumed = true;
        }
    }

    return consumed;
}

void dashboard_scene_settings_on_enter(void* context) {
    DashboardApp* app = context;

    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "Settings");
    submenu_add_item(
        app->submenu,
        "LCD",
        DashboardSettingsItemLcd,
        dashboard_scene_settings_submenu_callback,
        app);
    submenu_set_selected_item(
        app->submenu, scene_manager_get_scene_state(app->scene_manager, DashboardSceneSettings));

    view_dispatcher_switch_to_view(app->view_dispatcher, DashboardAppViewSubmenu);
}

void dashboard_scene_settings_on_exit(void* context) {
    DashboardApp* app = context;
    scene_manager_set_scene_state(
        app->scene_manager, DashboardSceneSettings, submenu_get_selected_item(app->submenu));
}
