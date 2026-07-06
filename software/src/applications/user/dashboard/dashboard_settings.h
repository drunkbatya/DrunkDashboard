#pragma once

#include <stdint.h>

typedef struct DashboardSettings DashboardSettings;

DashboardSettings* dashboard_settings_alloc(void);
void dashboard_settings_free(DashboardSettings* settings);
void dashboard_settings_apply(const DashboardSettings* settings);
void dashboard_settings_set_lcd_backlight(DashboardSettings* settings, uint8_t percent);
uint8_t dashboard_settings_get_lcd_backlight(const DashboardSettings* settings);
