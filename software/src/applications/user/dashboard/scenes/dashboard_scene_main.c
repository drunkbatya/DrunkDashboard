#include "../dashboard_app.h"
#include "../views/dashboard_view_main.h"
#include <furi.h>

//void dashboard_scene_main_button_callback(InputKey key, void* context) {
//    furi_assert(context);
//    DashboardApp* app = context;
//    view_dispatcher_send_custom_event(app->view_dispatcher, key);
//}

bool dashboard_scene_main_on_event(void* context, SceneManagerEvent event) {
    DashboardApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        //if(event.event == InputKeyLeft) {
        //    if(dashboard_view_is_idle_state(app->dashboard_view)) {
        //        dashboard_script_close(app->dashboard_script);
        //        app->dashboard_script = NULL;

        //        if(app->interface == DashboardHidInterfaceBle) {
        //            scene_manager_next_scene(app->scene_manager, DashboardSceneConfig);
        //        } else {
        //            scene_manager_next_scene(app->scene_manager, DashboardSceneConfigLayout);
        //        }
        //    }
        //    consumed = true;
        //} else if(event.event == InputKeyOk) {
        //    dashboard_script_start_stop(app->dashboard_script);
        //    consumed = true;
        //} else if(event.event == InputKeyRight) {
        //    if(dashboard_view_is_idle_state(app->dashboard_view)) {
        //        dashboard_set_interface(
        //            app,
        //            app->interface == DashboardHidInterfaceBle ? DashboardHidInterfaceUsb :
        //                                                      DashboardHidInterfaceBle);
        //        dashboard_script_close(app->dashboard_script);
        //        app->dashboard_script = dashboard_script_open(app->file_path, app->interface);
        //    } else {
        //        dashboard_script_pause_resume(app->dashboard_script);
        //    }
        //    consumed = true;
        //}
    } else if(event.type == SceneManagerEventTypeTick) {
        dashboard_view_main_update(app->view_main);
        //dashboard_view_set_state(app->dashboard_view, dashboard_script_get_state(app->dashboard_script));
    }
    return consumed;
}

void dashboard_scene_main_on_enter(void* context) {
    DashboardApp* app = context;

    //dashboard_view_set_interface(app->dashboard_view, app->interface);

    //app->dashboard_script = dashboard_script_open(app->file_path, app->interface);
    //dashboard_script_set_keyboard_layout(app->dashboard_script, app->keyboard_layout);

    //dashboard_view_set_state(app->dashboard_view, dashboard_script_get_state(app->dashboard_script));

    //dashboard_view_set_button_callback(app->dashboard_view, dashboard_scene_main_button_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, DashboardAppViewMain);
}

void dashboard_scene_main_on_exit(void* context) {
    UNUSED(context);
}
