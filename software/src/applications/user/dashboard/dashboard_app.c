#include "dashboard_app.h"

#include <stdlib.h>
#include <core/record.h>
#include <core/check.h>

#define TAG "Dashboard"

void dashboard_app_tick_event_callback(void* context) {
    furi_assert(context);
    DashboardApp* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

static bool dashboard_app_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    DashboardApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool dashboard_app_navigation_event_callback(void* context) {
    furi_assert(context);
    DashboardApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static DashboardApp* dashboard_app_alloc() {
    DashboardApp* app = malloc(sizeof(DashboardApp));

    app->gui = furi_record_open(RECORD_GUI);
    app->scene_manager = scene_manager_alloc(&dashboard_scene_handlers, app);
    app->settings = dashboard_settings_alloc();

    app->can_worker = dashboard_can_worker_alloc();
    app->can_frames = malloc(sizeof(DashboardCanFrame) * DASHBOARD_CAN_MAX_IDS);
    app->text = furi_string_alloc();
    dashboard_can_worker_start(app->can_worker);

    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, dashboard_app_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, dashboard_app_navigation_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, dashboard_app_tick_event_callback, 700);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    app->view_dashboard = dashboard_view_dashboard_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        DashboardAppViewDashboard,
        dashboard_view_dashboard_get_view(app->view_dashboard));

    app->view_menu = dashboard_view_menu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, DashboardAppViewMenu, dashboard_view_menu_get_view(app->view_menu));

    app->canbus_submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        DashboardAppViewCanbusSubmenu,
        submenu_get_view(app->canbus_submenu));

    app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, DashboardAppViewSubmenu, submenu_get_view(app->submenu));

    app->variable_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        DashboardAppViewVariableItemList,
        variable_item_list_get_view(app->variable_item_list));

    app->view_canbus = dashboard_view_canbus_view_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        DashboardAppViewCanbus,
        dashboard_view_canbus_get_view(app->view_canbus));

    scene_manager_next_scene(app->scene_manager, DashboardSceneDashboard);
    return app;
}

void dashboard_app_free(DashboardApp* app) {
    furi_assert(app);

    view_dispatcher_remove_view(app->view_dispatcher, DashboardAppViewDashboard);
    dashboard_view_dashboard_free(app->view_dashboard);

    view_dispatcher_remove_view(app->view_dispatcher, DashboardAppViewMenu);
    dashboard_view_menu_free(app->view_menu);

    view_dispatcher_remove_view(app->view_dispatcher, DashboardAppViewCanbusSubmenu);
    submenu_free(app->canbus_submenu);

    view_dispatcher_remove_view(app->view_dispatcher, DashboardAppViewSubmenu);
    submenu_free(app->submenu);

    view_dispatcher_remove_view(app->view_dispatcher, DashboardAppViewVariableItemList);
    variable_item_list_free(app->variable_item_list);

    view_dispatcher_remove_view(app->view_dispatcher, DashboardAppViewCanbus);
    dashboard_view_canbus_view_free(app->view_canbus);

    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    dashboard_can_worker_send_stop(app->can_worker);
    dashboard_can_worker_await_stop(app->can_worker);
    dashboard_can_worker_free(app->can_worker);
    dashboard_settings_free(app->settings);
    furi_string_free(app->text);
    free(app->can_frames);

    furi_record_close(RECORD_GUI);
    free(app);
}

int32_t dashboard_app(void* p) {
    UNUSED(p);
    FURI_LOG_I(TAG, "Start");
    DashboardApp* app = dashboard_app_alloc();

    FURI_LOG_I(TAG, "Run");
    view_dispatcher_run(app->view_dispatcher);

    dashboard_app_free(app);
    return 0;
}
