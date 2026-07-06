#pragma once

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/variable_item_list.h>

#include "scenes/dashboard_scene.h"

#include "dashboard_settings.h"
#include "views/dashboard_view_dashboard.h"
#include "views/dashboard_view_menu.h"
#include "views/dashboard_view_canbus.h"

#include "workers/dashboard_can_worker.h"

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    Submenu* canbus_submenu;
    Submenu* submenu;
    VariableItemList* variable_item_list;
    DashboardViewDashboard* view_dashboard;
    DashboardViewMenu* view_menu;
    DashboardViewCanbus* view_canbus;
    DashboardCanWorker* can_worker;
    DashboardCanFrame* can_frames;
    FuriString* text;
    size_t can_frames_count;
    uint32_t selected_can_id;
    DashboardSettings* settings;
} DashboardApp;

typedef enum {
    DashboardAppViewDashboard,
    DashboardAppViewMenu,
    DashboardAppViewCanbusSubmenu,
    DashboardAppViewCanbus,
    DashboardAppViewSubmenu,
    DashboardAppViewVariableItemList,
} DashboardAppView;

typedef enum {
    DashboardAppCustomEventDataUpdated,
    DashboardAppCustomEventCanIdSelected,
    DashboardAppCustomEventMenuSelected,
    DashboardAppCustomEventSettingsSelected,
} DashboardAppCustomEvent;

int32_t dashboard_app(void* p);
