#include "../dashboard_app.h"

#include <dialogs/dialogs.h>
#include <furi.h>
#include <furi_hal_version.h>
#include <gui/modules/empty_screen.h>
#include <gui/view_holder.h>

typedef DialogMessageButton (
    *DashboardAboutScreen)(DashboardApp* app, DialogsApp* dialogs, DialogMessage* message);

static DialogMessageButton dashboard_about_screen_hw_version(
    DashboardApp* app,
    DialogsApp* dialogs,
    DialogMessage* message) {
    furi_string_reset(app->text);

    furi_string_cat_printf(
        app->text,
        "Model: %s [%s]\n",
        furi_hal_version_get_model_name(),
        furi_hal_version_get_model_code());

    furi_string_cat_printf(app->text, "Serial Number:\n");
    const uint8_t* uid = furi_hal_version_uid();
    for(size_t i = 0; i < furi_hal_version_uid_size(); i++) {
        furi_string_cat_printf(app->text, "%02X", uid[i]);
    }

    dialog_message_set_header(message, "HW Version Info:", 0, 0, AlignLeft, AlignTop);
    dialog_message_set_text(message, furi_string_get_cstr(app->text), 0, 13, AlignLeft, AlignTop);
    return dialog_message_show(dialogs, message);
}

static DialogMessageButton dashboard_about_screen_fw_version(
    DashboardApp* app,
    DialogsApp* dialogs,
    DialogMessage* message) {
    const Version* version = furi_hal_version_get_firmware_version();
    furi_string_reset(app->text);

    if(version) {
        furi_string_cat_printf(
            app->text,
            "%s [%s]\n"
            "%s%s\n"
            "[%u] %s\n",
            version_get_version(version),
            version_get_builddate(version),
            version_get_dirty_flag(version) ? "[!] " : "",
            version_get_githash(version),
            version_get_target(version),
            version_get_gitbranch(version));
    } else {
        furi_string_cat(app->text, "No info");
    }

    dialog_message_set_header(message, "FW Version Info:", 0, 0, AlignLeft, AlignTop);
    dialog_message_set_text(message, furi_string_get_cstr(app->text), 0, 13, AlignLeft, AlignTop);
    return dialog_message_show(dialogs, message);
}

static const DashboardAboutScreen dashboard_about_screens[] = {
    dashboard_about_screen_hw_version,
    dashboard_about_screen_fw_version,
};

bool dashboard_scene_settings_about_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void dashboard_scene_settings_about_on_enter(void* context) {
    DashboardApp* app = context;
    DialogsApp* dialogs = furi_record_open(RECORD_DIALOGS);
    DialogMessage* message = dialog_message_alloc();

    Gui* gui = furi_record_open(RECORD_GUI);
    ViewHolder* view_holder = view_holder_alloc();
    EmptyScreen* empty_screen = empty_screen_alloc();
    view_holder_attach_to_gui(view_holder, gui);
    view_holder_set_view(view_holder, empty_screen_get_view(empty_screen));

    size_t screen_index =
        scene_manager_get_scene_state(app->scene_manager, DashboardSceneSettingsAbout);
    if(screen_index >= COUNT_OF(dashboard_about_screens)) {
        screen_index = 0;
    }

    while(1) {
        if(screen_index >= COUNT_OF(dashboard_about_screens) - 1) {
            dialog_message_set_buttons(message, "Prev.", NULL, NULL);
        } else if(screen_index == 0) {
            dialog_message_set_buttons(message, NULL, NULL, "Next");
        } else {
            dialog_message_set_buttons(message, "Prev.", NULL, "Next");
        }

        DialogMessageButton screen_result =
            dashboard_about_screens[screen_index](app, dialogs, message);

        dialog_message_set_icon(message, NULL, 0, 0);
        dialog_message_set_header(message, NULL, 0, 0, AlignLeft, AlignTop);
        dialog_message_set_text(message, NULL, 0, 0, AlignLeft, AlignTop);

        if(screen_result == DialogMessageButtonLeft) {
            if(screen_index == 0) {
                break;
            }
            screen_index--;
        } else if(screen_result == DialogMessageButtonRight) {
            if(screen_index < COUNT_OF(dashboard_about_screens) - 1) {
                screen_index++;
            }
        } else if(screen_result == DialogMessageButtonBack) {
            break;
        }
    }

    scene_manager_set_scene_state(app->scene_manager, DashboardSceneSettingsAbout, screen_index);
    dialog_message_free(message);
    furi_record_close(RECORD_DIALOGS);

    view_holder_set_view(view_holder, NULL);
    view_holder_free(view_holder);
    empty_screen_free(empty_screen);
    furi_record_close(RECORD_GUI);

    scene_manager_previous_scene(app->scene_manager);
}

void dashboard_scene_settings_about_on_exit(void* context) {
    UNUSED(context);
}
