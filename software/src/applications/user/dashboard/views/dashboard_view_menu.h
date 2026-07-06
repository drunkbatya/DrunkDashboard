#pragma once

#include <gui/icon.h>
#include <gui/view.h>
#include <stdint.h>

typedef struct DashboardViewMenu DashboardViewMenu;
typedef void (*DashboardViewMenuCallback)(void* context, uint32_t index);

DashboardViewMenu* dashboard_view_menu_alloc(void);
void dashboard_view_menu_free(DashboardViewMenu* menu);
View* dashboard_view_menu_get_view(DashboardViewMenu* menu);
void dashboard_view_menu_reset(DashboardViewMenu* menu);
void dashboard_view_menu_add_item(
    DashboardViewMenu* menu,
    const char* label,
    const Icon* icon,
    uint32_t index);
void dashboard_view_menu_set_callback(
    DashboardViewMenu* menu,
    DashboardViewMenuCallback callback,
    void* context);
void dashboard_view_menu_set_selected_item(DashboardViewMenu* menu, uint32_t index);
uint32_t dashboard_view_menu_get_selected_item(DashboardViewMenu* menu);
