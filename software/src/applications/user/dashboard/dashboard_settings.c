#include "dashboard_settings_i.h"

#include <furi_hal_backlight.h>
#include <stdlib.h>

#define DASHBOARD_SETTINGS_LCD_BACKLIGHT_DEFAULT 75

DashboardSettings* dashboard_settings_alloc(void) {
    DashboardSettings* settings = malloc(sizeof(DashboardSettings));
    settings->lcd_backlight_percent = DASHBOARD_SETTINGS_LCD_BACKLIGHT_DEFAULT;
    dashboard_settings_apply(settings);
    return settings;
}

void dashboard_settings_free(DashboardSettings* settings) {
    free(settings);
}

void dashboard_settings_apply(const DashboardSettings* settings) {
    furi_hal_backlight_set(settings->lcd_backlight_percent);
}

void dashboard_settings_set_lcd_backlight(DashboardSettings* settings, uint8_t percent) {
    if(percent > 100) percent = 100;
    settings->lcd_backlight_percent = percent;
    furi_hal_backlight_set(percent);
}

uint8_t dashboard_settings_get_lcd_backlight(const DashboardSettings* settings) {
    return settings->lcd_backlight_percent;
}
