#include "../dashboard_app.h"

#include <assets_icons.h>
#include <furi.h>

typedef enum {
    DashboardMenuItemDashboard,
    DashboardMenuItemCanSniffer,
    DashboardMenuItemSettings,
} DashboardMenuItem;

static void dashboard_scene_menu_view_callback(void* context, uint32_t index) {
    DashboardApp* app = context;
    scene_manager_set_scene_state(app->scene_manager, DashboardSceneMenu, index);
    view_dispatcher_send_custom_event(app->view_dispatcher, DashboardAppCustomEventMenuSelected);
}

bool dashboard_scene_menu_on_event(void* context, SceneManagerEvent event) {
    DashboardApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == DashboardAppCustomEventMenuSelected) {
            uint32_t selected =
                scene_manager_get_scene_state(app->scene_manager, DashboardSceneMenu);
            if(selected == DashboardMenuItemDashboard) {
                scene_manager_previous_scene(app->scene_manager);
            } else if(selected == DashboardMenuItemCanSniffer) {
                scene_manager_next_scene(app->scene_manager, DashboardSceneCanbus);
            } else if(selected == DashboardMenuItemSettings) {
                scene_manager_next_scene(app->scene_manager, DashboardSceneSettings);
            }
            consumed = true;
        }
    }

    return consumed;
}

void dashboard_scene_menu_on_enter(void* context) {
    DashboardApp* app = context;
    dashboard_view_menu_set_callback(app->view_menu, dashboard_scene_menu_view_callback, app);
    dashboard_view_menu_reset(app->view_menu);
    dashboard_view_menu_add_item(
        app->view_menu, "Dashboard", &A_Plugins_14, DashboardMenuItemDashboard);
    dashboard_view_menu_add_item(
        app->view_menu, "CAN Sniffer", &A_GPIO_14, DashboardMenuItemCanSniffer);
    dashboard_view_menu_add_item(
        app->view_menu, "Settings", &A_Settings_14, DashboardMenuItemSettings);
    dashboard_view_menu_set_selected_item(
        app->view_menu, scene_manager_get_scene_state(app->scene_manager, DashboardSceneMenu));
    view_dispatcher_switch_to_view(app->view_dispatcher, DashboardAppViewMenu);
}

void dashboard_scene_menu_on_exit(void* context) {
    DashboardApp* app = context;
    scene_manager_set_scene_state(
        app->scene_manager,
        DashboardSceneMenu,
        dashboard_view_menu_get_selected_item(app->view_menu));
    dashboard_view_menu_set_callback(app->view_menu, NULL, NULL);
    dashboard_view_menu_reset(app->view_menu);
}
