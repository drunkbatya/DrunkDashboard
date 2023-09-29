#include "dashboard.h"
#include <stdbool.h>
#include <system_delay.h>
#include <system_thread.h>
#include <u8g2/u8g2_glue.h>
#include <cmsis_os2.h>

#include <stdlib.h>

static Dashboard* dashboard_alloc() {
    Dashboard* dashboard = malloc(sizeof(Dashboard));
    dashboard->scene_manager =
        scene_manager_alloc(&dashboard_scene_handlers, dashboard);
    dashboard->gui = gui_alloc();
    dashboard->view_pressure = dashboard_view_pressure_alloc();
    dashboard->view_debug = dashboard_view_debug_alloc();
    return dashboard;
}

static void dashboard_free(Dashboard* dashboard) {
    dashboard_view_debug_free(dashboard->view_debug);
    dashboard_view_pressure_free(dashboard->view_pressure);
    gui_free(dashboard->gui);
    scene_manager_free(dashboard->scene_manager);
    free(dashboard);
}

static void dashboard_thread(void* context) {
    Dashboard* dashboard = context;
    scene_manager_next_scene(dashboard->scene_manager, DashboardSceneMain);
    uint16_t pressure = 200;
    while(true) {
        dashboard_view_pressure_set_pressure(dashboard->view_pressure, pressure);
        gui_draw(dashboard->gui);
        pressure++;
        if(pressure >= 350) pressure = 0;
    }
}

void init_thread() {
    Dashboard* dashboard = dashboard_alloc();
    dashboard_thread(dashboard);
    dashboard_free(dashboard);
}
