#include "../dashboard_app.h"

#include <furi.h>

#define DASHBOARD_LCD_BACKLIGHT_COUNT 21

static const char* const dashboard_lcd_backlight_text[DASHBOARD_LCD_BACKLIGHT_COUNT] = {
    "0%",  "5%",  "10%", "15%", "20%", "25%", "30%", "35%", "40%", "45%",  "50%",
    "55%", "60%", "65%", "70%", "75%", "80%", "85%", "90%", "95%", "100%",
};

static void dashboard_scene_settings_lcd_backlight_changed(VariableItem* item) {
    DashboardApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, dashboard_lcd_backlight_text[index]);
    dashboard_settings_set_lcd_backlight(app->settings, index * 5);
}

bool dashboard_scene_settings_lcd_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void dashboard_scene_settings_lcd_on_enter(void* context) {
    DashboardApp* app = context;

    variable_item_list_reset(app->variable_item_list);
    VariableItem* item = variable_item_list_add(
        app->variable_item_list,
        "LCD Backlight",
        DASHBOARD_LCD_BACKLIGHT_COUNT,
        dashboard_scene_settings_lcd_backlight_changed,
        app);

    uint8_t index = dashboard_settings_get_lcd_backlight(app->settings) / 5;
    if(index >= DASHBOARD_LCD_BACKLIGHT_COUNT) index = DASHBOARD_LCD_BACKLIGHT_COUNT - 1;
    variable_item_set_current_value_index(item, index);
    variable_item_set_current_value_text(item, dashboard_lcd_backlight_text[index]);

    view_dispatcher_switch_to_view(app->view_dispatcher, DashboardAppViewVariableItemList);
}

void dashboard_scene_settings_lcd_on_exit(void* context) {
    DashboardApp* app = context;
    variable_item_list_reset(app->variable_item_list);
}
